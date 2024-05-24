/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_NET_BRAVE_NETWORK_AUDIT_ALLOWED_LISTS_H_
#define BRAVE_BROWSER_NET_BRAVE_NETWORK_AUDIT_ALLOWED_LISTS_H_

namespace brave {

// Before adding to this list, get approval from the security team.
inline constexpr const char* kAllowedUrlProtocols[] = {
    "chrome-extension", "chrome", "brave", "file", "data", "blob",
};

// Before adding to this list, get approval from the security team.
inline constexpr const char* kAllowedUrlPrefixes[] = {
    // allowed because it 307's to https://componentupdater.browseweb3.com
    "https://componentupdater.browseweb3.com/service/update2",
    "https://crxdownload.browseweb3.com/crx/blobs/",

    // Omaha/Sparkle
    "https://updates.bravesoftware.com/",

    // stats/referrals
    "https://laptop-updates.browseweb3.com/",
    "https://laptop-updates-staging.browseweb3.com/",

    // needed for DoH on Mac build machines
    "https://dns.google/dns-query",

    // needed for DoH on Mac build machines
    "https://chrome.cloudflare-dns.com/dns-query",

    // for fetching tor client updater component
    "https://tor.bravesoftware.com/",

    // brave sync v2 production
    "https://sync-v2.browseweb3.com/v2",

    // brave sync v2 staging
    "https://sync-v2.bravesoftware.com/v2",

    // brave sync v2 dev
    "https://sync-v2.brave.software/v2",

    // brave A/B testing
    "https://variations.browseweb3.com/seed",

    // Brave News (production)
    "https://brave-today-cdn.browseweb3.com/",

    // Brave's Privacy-focused CDN
    "https://pcdn.browseweb3.com/",

    // Brave Rewards production
    "https://api.rewards.browseweb3.com/v1/parameters",
    "https://rewards.browseweb3.com/publishers/prefix-list",
    "https://grant.rewards.browseweb3.com/v1/promotions",

    // Brave Rewards staging & dev
    "https://api.rewards.bravesoftware.com/v1/parameters",
    "https://rewards-stg.bravesoftware.com/publishers/prefix-list",
    "https://grant.rewards.bravesoftware.com/v1/promotions",

    // p3a
    "https://p3a-creative.browseweb3.com/",
    "https://p3a-json.browseweb3.com/",
    "https://p3a.browseweb3.com/",
    "https://star-randsrv.bsg.browseweb3.com/",

    // Other
    "https://brave-core-ext.s3.browseweb3.com/",
    "https://dict.browseweb3.com/",
    "https://go-updater.browseweb3.com/",
    "https://redirector.browseweb3.com/",
    "https://safebrowsing.browseweb3.com/",
    "https://static.browseweb3.com/",
    "https://static1.browseweb3.com/",
};

// Before adding to this list, get approval from the security team.
inline constexpr const char* kAllowedUrlPatterns[] = {
    // allowed because it's url for fetching super referral's mapping table
    "https://mobile-data.s3.browseweb3.com/superreferrer/map-table.json",
    "https://mobile-data-dev.s3.brave.software/superreferrer/map-table.json",
};

}  // namespace brave

#endif  // BRAVE_BROWSER_NET_BRAVE_NETWORK_AUDIT_ALLOWED_LISTS_H_
