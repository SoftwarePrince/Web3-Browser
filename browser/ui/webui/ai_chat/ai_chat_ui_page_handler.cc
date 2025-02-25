// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/webui/ai_chat/ai_chat_ui_page_handler.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "base/notreached.h"
#include "base/strings/utf_string_conversions.h"
#include "brave/common/brave_channel_info.h"
#include "brave/components/ai_chat/core/browser/constants.h"
#include "brave/components/ai_chat/core/browser/models.h"
#include "brave/components/ai_chat/core/common/mojom/ai_chat.mojom-shared.h"
#include "brave/components/ai_chat/core/common/mojom/ai_chat.mojom.h"
#include "brave/components/ai_chat/core/common/pref_names.h"
#include "chrome/browser/favicon/favicon_service_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/singleton_tabs.h"
#include "components/favicon/core/favicon_service.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/visibility.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/common/url_constants.h"
#include "ui/base/l10n/l10n_util.h"

#if BUILDFLAG(IS_ANDROID)
#include "brave/browser/ui/android/ai_chat/brave_leo_settings_launcher_helper.h"
#endif

namespace {
constexpr uint32_t kDesiredFaviconSizePixels = 32;
}  // namespace

namespace ai_chat {

using mojom::CharacterType;
using mojom::ConversationTurn;
using mojom::ConversationTurnVisibility;

AIChatUIPageHandler::AIChatUIPageHandler(
    content::WebContents* owner_web_contents,
    content::WebContents* chat_context_web_contents,
    Profile* profile,
    mojo::PendingReceiver<ai_chat::mojom::PageHandler> receiver)
    : content::WebContentsObserver(owner_web_contents),
      profile_(profile),
      receiver_(this, std::move(receiver)) {
  // Standalone mode means Chat is opened as its own tab in the tab strip and
  // not a side panel. chat_context_web_contents is nullptr in that case
  const bool is_standalone = chat_context_web_contents == nullptr;
  if (!is_standalone) {
    active_chat_tab_helper_ =
        ai_chat::AIChatTabHelper::FromWebContents(chat_context_web_contents);
    chat_tab_helper_observation_.Observe(active_chat_tab_helper_);
    // Report visibility of AI Chat UI to the Conversation, so that
    // automatic actions are only performed when neccessary.
    bool is_visible =
        (owner_web_contents->GetVisibility() == content::Visibility::VISIBLE)
            ? true
            : false;
    active_chat_tab_helper_->OnConversationActiveChanged(is_visible);
  } else {
    // TODO(petemill): Enable conversation without the TabHelper. Conversation
    // logic should be extracted from the TabHelper to a new virtual class, e.g.
    // AIChatConverser, that the TabHelper can implement and a
    // StandaloneAIChatConverser can also implement and be instantiated here.
    NOTIMPLEMENTED();
  }

  favicon_service_ = FaviconServiceFactory::GetForProfile(
      profile_, ServiceAccessType::EXPLICIT_ACCESS);

  feedback_api_ = std::make_unique<AIChatFeedbackAPI>(
      owner_web_contents->GetBrowserContext()
          ->GetDefaultStoragePartition()
          ->GetURLLoaderFactoryForBrowserProcess(),
      brave::GetChannelName());
}

AIChatUIPageHandler::~AIChatUIPageHandler() = default;

void AIChatUIPageHandler::SetClientPage(
    mojo::PendingRemote<ai_chat::mojom::ChatUIPage> page) {
  page_.Bind(std::move(page));

  // In some cases, this page handler hasn't been created and remote might not
  // have been set yet.
  // ex. A user may ask a question from the location bar
  if (active_chat_tab_helper_ &&
      active_chat_tab_helper_->HasPendingConversationEntry()) {
    OnHistoryUpdate();
  }
}

void AIChatUIPageHandler::GetModels(GetModelsCallback callback) {
  if (!active_chat_tab_helper_) {
    VLOG(2) << "Chat tab helper is not set";
    std::move(callback).Run(std::vector<mojom::ModelPtr>(), std::string());
    return;
  }

  auto all_models = GetAllModels();
  std::vector<mojom::ModelPtr> models(all_models.size());
  // Ensure we return only in intended display order
  std::transform(all_models.cbegin(), all_models.cend(), models.begin(),
                 [](auto& model) { return model.Clone(); });

  std::move(callback).Run(std::move(models),
                          active_chat_tab_helper_->GetCurrentModel().key);
}

void AIChatUIPageHandler::ChangeModel(const std::string& model_key) {
  active_chat_tab_helper_->ChangeModel(model_key);
}

void AIChatUIPageHandler::SubmitHumanConversationEntry(
    const std::string& input) {
  DCHECK(!active_chat_tab_helper_->IsRequestInProgress())
      << "Should not be able to submit more"
      << "than a single human conversation turn at a time.";
  mojom::ConversationTurn turn = {CharacterType::HUMAN,
                                  ConversationTurnVisibility::VISIBLE, input};
  active_chat_tab_helper_->SubmitHumanConversationEntry(std::move(turn));
}

void AIChatUIPageHandler::SubmitSummarizationRequest() {
  if (active_chat_tab_helper_) {
    active_chat_tab_helper_->SubmitSummarizationRequest();
  }
}

void AIChatUIPageHandler::GetConversationHistory(
    GetConversationHistoryCallback callback) {
  if (!active_chat_tab_helper_) {
    std::move(callback).Run({});
    return;
  }

  std::move(callback).Run(
      active_chat_tab_helper_->GetVisibleConversationHistory());
}

void AIChatUIPageHandler::GetSuggestedQuestions(
    GetSuggestedQuestionsCallback callback) {
  if (!active_chat_tab_helper_) {
    std::move(callback).Run({}, mojom::SuggestionGenerationStatus::None);
    return;
  }
  mojom::SuggestionGenerationStatus suggestion_status;
  std::move(callback).Run(
      active_chat_tab_helper_->GetSuggestedQuestions(suggestion_status),
      suggestion_status);
}

void AIChatUIPageHandler::GenerateQuestions() {
  if (active_chat_tab_helper_) {
    active_chat_tab_helper_->GenerateQuestions();
  }
}

void AIChatUIPageHandler::GetSiteInfo(GetSiteInfoCallback callback) {
  if (!active_chat_tab_helper_) {
    VLOG(2) << "Chat tab helper is not set";
    std::move(callback).Run({});
    return;
  }

  auto site_info = active_chat_tab_helper_->BuildSiteInfo();
  std::move(callback).Run(site_info.Clone());
}

void AIChatUIPageHandler::OpenBraveLeoSettings() {
  auto* contents_to_navigate = (active_chat_tab_helper_)
                                   ? active_chat_tab_helper_->web_contents()
                                   : web_contents();
#if !BUILDFLAG(IS_ANDROID)
  const GURL url("browseweb3://settings/leo-assistant");
  if (auto* browser = chrome::FindBrowserWithTab(contents_to_navigate)) {
    ShowSingletonTab(browser, url);
  } else {
    contents_to_navigate->OpenURL({url, content::Referrer(),
                                   WindowOpenDisposition::NEW_FOREGROUND_TAB,
                                   ui::PAGE_TRANSITION_LINK, false});
  }
#else
  ai_chat::ShowBraveLeoSettings(contents_to_navigate);
#endif
}

void AIChatUIPageHandler::OpenURL(const GURL& url) {
  if (!url.SchemeIs(content::kChromeUIScheme) &&
      !url.SchemeIs(url::kHttpsScheme)) {
    return;
  }

#if !BUILDFLAG(IS_ANDROID)
  auto* contents_to_navigate = (active_chat_tab_helper_)
                                   ? active_chat_tab_helper_->web_contents()
                                   : web_contents();
  contents_to_navigate->OpenURL({url, content::Referrer(),
                                 WindowOpenDisposition::NEW_FOREGROUND_TAB,
                                 ui::PAGE_TRANSITION_LINK, false});
#else
  // TODO(sergz): Disable subscriptions on Android until in store purchases
  // are done.
  return;
#endif
}

void AIChatUIPageHandler::SetShouldSendPageContents(bool should_send) {
  if (active_chat_tab_helper_) {
    active_chat_tab_helper_->SetShouldSendPageContents(should_send);
  }
}

void AIChatUIPageHandler::GetShouldSendPageContents(
    GetShouldSendPageContentsCallback callback) {
  if (active_chat_tab_helper_) {
    std::move(callback).Run(
        active_chat_tab_helper_->GetShouldSendPageContents());
  }
}

void AIChatUIPageHandler::ClearConversationHistory() {
  if (active_chat_tab_helper_) {
    active_chat_tab_helper_->ClearConversationHistory();
  }
}

void AIChatUIPageHandler::RetryAPIRequest() {
  if (active_chat_tab_helper_) {
    active_chat_tab_helper_->RetryAPIRequest();
  }
}

void AIChatUIPageHandler::GetAPIResponseError(
    GetAPIResponseErrorCallback callback) {
  if (!active_chat_tab_helper_) {
    std::move(callback).Run(mojom::APIError::None);
    return;
  }
  std::move(callback).Run(active_chat_tab_helper_->GetCurrentAPIError());
}

void AIChatUIPageHandler::GetCanShowPremiumPrompt(
    GetCanShowPremiumPromptCallback callback) {
  bool has_user_dismissed_prompt = profile_->GetPrefs()->GetBoolean(
      ai_chat::prefs::kUserDismissedPremiumPrompt);

  if (has_user_dismissed_prompt) {
    std::move(callback).Run(false);
    return;
  }

  base::Time last_accepted_disclaimer =
      profile_->GetPrefs()->GetTime(ai_chat::prefs::kLastAcceptedDisclaimer);

  // Can't show if we haven't accepted disclaimer yet
  if (last_accepted_disclaimer.is_null()) {
    std::move(callback).Run(false);
    return;
  }

  base::Time time_1_day_ago = base::Time::Now() - base::Days(1);
  bool is_more_than_24h_since_last_seen =
      last_accepted_disclaimer < time_1_day_ago;

  if (is_more_than_24h_since_last_seen) {
    std::move(callback).Run(true);
    return;
  }

  std::move(callback).Run(false);
}

void AIChatUIPageHandler::DismissPremiumPrompt() {
  profile_->GetPrefs()->SetBoolean(ai_chat::prefs::kUserDismissedPremiumPrompt,
                                   true);
}

void AIChatUIPageHandler::RateMessage(bool is_liked,
                                      uint32_t turn_id,
                                      RateMessageCallback callback) {
  auto on_complete = base::BindOnce(
      [](RateMessageCallback callback, APIRequestResult result) {
        if (result.Is2XXResponseCode() && result.value_body().is_dict()) {
          std::string id = *result.value_body().GetDict().FindString("id");
          std::move(callback).Run(id);
          return;
        }
        std::move(callback).Run(absl::nullopt);
      },
      std::move(callback));

  if (active_chat_tab_helper_) {
    const std::vector<mojom::ConversationTurn>& history =
        active_chat_tab_helper_->GetConversationHistory();

    // TODO(petemill): Something more robust than relying on message index,
    // and probably a message uuid.
    uint32_t current_turn_id = turn_id + 1;

    if (current_turn_id <= history.size()) {
      base::span<const mojom::ConversationTurn> history_slice =
          base::make_span(history).first(current_turn_id);

      feedback_api_->SendRating(is_liked, history_slice,
                                active_chat_tab_helper_->GetCurrentModel().name,
                                std::move(on_complete));

      return;
    }
  }

  std::move(callback).Run(absl::nullopt);
}

void AIChatUIPageHandler::SendFeedback(const std::string& category,
                                       const std::string& feedback,
                                       const std::string& rating_id,
                                       SendFeedbackCallback callback) {
  auto on_complete = base::BindOnce(
      [](SendFeedbackCallback callback, APIRequestResult result) {
        if (result.Is2XXResponseCode()) {
          std::move(callback).Run(true);
          return;
        }

        std::move(callback).Run(false);
      },
      std::move(callback));

  feedback_api_->SendFeedback(category, feedback, rating_id,
                              std::move(on_complete));
}

void AIChatUIPageHandler::MarkAgreementAccepted() {
  profile_->GetPrefs()->SetTime(ai_chat::prefs::kLastAcceptedDisclaimer,
                                base::Time::Now());
}

void AIChatUIPageHandler::OnHistoryUpdate() {
  if (page_.is_bound()) {
    page_->OnConversationHistoryUpdate();
  }
}

void AIChatUIPageHandler::OnAPIRequestInProgress(bool in_progress) {
  if (page_.is_bound()) {
    page_->OnAPIRequestInProgress(in_progress);
  }
}

void AIChatUIPageHandler::OnAPIResponseError(mojom::APIError error) {
  if (page_.is_bound()) {
    page_->OnAPIResponseError(error);
  }
}

void AIChatUIPageHandler::OnModelChanged(const std::string& model_key) {
  if (page_.is_bound()) {
    page_->OnModelChanged(model_key);
  }
}

void AIChatUIPageHandler::OnSuggestedQuestionsChanged(
    std::vector<std::string> questions,
    mojom::SuggestionGenerationStatus suggestion_generation_status) {
  if (page_.is_bound()) {
    page_->OnSuggestedQuestionsChanged(std::move(questions),
                                       suggestion_generation_status);
  }
}

void AIChatUIPageHandler::OnFaviconImageDataChanged() {
  if (page_.is_bound()) {
    auto on_favicon_data =
        [](base::SafeRef<AIChatUIPageHandler> page_handler,
           const absl::optional<std::vector<uint8_t>>& bytes) {
          if (bytes.has_value()) {
            page_handler->page_->OnFaviconImageDataChanged(bytes.value());
          }
        };

    GetFaviconImageData(
        base::BindOnce(on_favicon_data, weak_ptr_factory_.GetSafeRef()));
  }
}

void AIChatUIPageHandler::OnPageHasContent(mojom::SiteInfoPtr site_info) {
  if (page_.is_bound()) {
    page_->OnSiteInfoChanged(std::move(site_info));
  }
}

void AIChatUIPageHandler::GetFaviconImageData(
    GetFaviconImageDataCallback callback) {
  if (!active_chat_tab_helper_) {
    std::move(callback).Run(absl::nullopt);
    return;
  }

  const GURL active_page_url =
      active_chat_tab_helper_->web_contents()->GetLastCommittedURL();
  favicon_base::IconTypeSet icon_types{favicon_base::IconType::kFavicon,
                                       favicon_base::IconType::kTouchIcon};

  auto on_favicon_available =
      [](GetFaviconImageDataCallback callback,
         const favicon_base::FaviconRawBitmapResult& result) {
        if (!result.is_valid()) {
          std::move(callback).Run(absl::nullopt);
          return;
        }

        scoped_refptr<base::RefCountedMemory> bytes = result.bitmap_data;
        std::vector<uint8_t> buffer(bytes->front_as<uint8_t>(),
                                    bytes->front_as<uint8_t>() + bytes->size());
        std::move(callback).Run(std::move(buffer));
      };

  favicon_service_->GetRawFaviconForPageURL(
      active_page_url, icon_types, kDesiredFaviconSizePixels, true,
      base::BindOnce(on_favicon_available, std::move(callback)),
      &favicon_task_tracker_);
}

void AIChatUIPageHandler::OnVisibilityChanged(content::Visibility visibility) {
  // WebUI visibility changed (not target tab)
  if (!active_chat_tab_helper_) {
    return;
  }
  bool is_visible = (visibility == content::Visibility::VISIBLE) ? true : false;
  active_chat_tab_helper_->OnConversationActiveChanged(is_visible);
}

void AIChatUIPageHandler::GetPremiumStatus(GetPremiumStatusCallback callback) {
  if (!active_chat_tab_helper_) {
    VLOG(2) << "Chat tab helper is not set";
    std::move(callback).Run(mojom::PremiumStatus::Inactive);
    return;
  }

  active_chat_tab_helper_->GetPremiumStatus(std::move(callback));
}

}  // namespace ai_chat
