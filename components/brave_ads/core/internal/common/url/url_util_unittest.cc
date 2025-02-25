/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/common/url/url_util.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

namespace brave_ads {

TEST(BraveAdsUrlUtilTest, GetUrlWithEmptyQuery) {
  // Act & Assert
  EXPECT_EQ(GURL("https://foo.com/bar"),
            GetUrlWithEmptyQuery(GURL("https://foo.com/bar?baz=qux")));
}

TEST(BraveAdsUrlUtilTest, DoesNotSupportInvalidUrl) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("INVALID")));
}

TEST(BraveAdsUrlUtilTest, DoesSupportUrlWithHttpsScheme) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("https://foobar.com")));
}

TEST(BraveAdsUrlUtilTest, DoesNotSupportUrlWithHttpScheme) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("http://foobar.com")));
}

TEST(BraveAdsUrlUtilTest, DoesNotSupportUrlWithFooBarScheme) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("foobar://baz")));
}

TEST(BraveAdsUrlUtilTest, DoesNotSupportBraveSchemeWithFooBarHostName) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("browseweb3://foobar")));
}

TEST(BraveAdsUrlUtilTest, DoesSupportBraveSchemeWithWalletHostName) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://wallet")));
}

TEST(BraveAdsUrlUtilTest, DoesSupportBraveSchemeWithWalletHostNameAndPath) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://wallet/foo")));
}

TEST(BraveAdsUrlUtilTest, DoesSupportBraveSchemeWithSyncHostName) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://sync")));
}

TEST(BraveAdsUrlUtilTest, DoesSupportBraveSchemeWithSyncHostNameAndPath) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://sync/foo")));
}

TEST(BraveAdsUrlUtilTest, DoesSupportBraveSchemeWithRewardsHostName) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://rewards")));
}

TEST(BraveAdsUrlUtilTest, DoesSupportBraveSchemeWithRewardsHostNameAndPath) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://rewards/foo")));
}

TEST(BraveAdsUrlUtilTest, DoesNotSupportBraveSchemeWithSettingsHostName) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("browseweb3://settings")));
}

TEST(BraveAdsUrlUtilTest,
     DoesNotSupportBraveSchemeWithSettingsHostNameAndFooBarPath) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("browseweb3://settings/foobar")));
}

TEST(BraveAdsUrlUtilTest,
     DoesSupportBraveSchemeWithSettingsHostNameAndSearchEnginesPath) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://settings/searchEngines")));
}

TEST(
    BraveAdsUrlUtilTest,
    DoesSupportBraveSchemeWithSettingsHostNameSearchEnginesPathAndSearchQuery) {
  // Act & Assert
  EXPECT_TRUE(
      DoesSupportUrl(GURL("browseweb3://settings/searchEngines?search=foobar")));
}

TEST(
    BraveAdsUrlUtilTest,
    DoesNotSupportBraveSchemeWithSettingsHostNameSearchEnginesPathAndMultipleSearchQueries) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(
      GURL("browseweb3://settings/searchEngines?search=foo&bar=baz")));
}

TEST(
    BraveAdsUrlUtilTest,
    DoesNotSupportBraveSchemeWithSettingsHostNameSearchEnginesPathAndInvalidQuery) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("browseweb3://settings/searchEngines?search")));
}

TEST(BraveAdsUrlUtilTest,
     DoesSupportBraveSchemeWithSettingsHostNameAndSearchPath) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://settings/search")));
}

TEST(BraveAdsUrlUtilTest,
     DoesSupportBraveSchemeWithSettingsHostNameSearchPathAndSearchQuery) {
  // Act & Assert
  EXPECT_TRUE(DoesSupportUrl(GURL("browseweb3://settings/search?search=foobar")));
}

TEST(
    BraveAdsUrlUtilTest,
    DoesNotSupportBraveSchemeWithSettingsHostNameSearchPathAndMultipleSearchQueries) {
  // Act & Assert
  EXPECT_FALSE(
      DoesSupportUrl(GURL("browseweb3://settings/search?search=foo&bar=baz")));
}

TEST(BraveAdsUrlUtilTest,
     DoesNotSupportBraveSchemeWithSettingsHostNameSearchPathAndInvalidQuery) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("browseweb3://settings/search?search")));
}

TEST(BraveAdsUrlUtilTest,
     DoesNotSupportBraveSchemeWithSettingsHostNameAndQuery) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("browseweb3://settings/?search=foobar")));
}

TEST(BraveAdsUrlUtilTest,
     DoesNotSupportBraveSchemeWithSettingsHostNameAndInvalidQuery) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("browseweb3://settings/?search")));
}

TEST(BraveAdsUrlUtilTest, MalformedUrlIsNotSupported) {
  // Act & Assert
  EXPECT_FALSE(DoesSupportUrl(GURL("http://foobar.com/browseweb3://wallet")));
}

TEST(BraveAdsUrlUtilTest, UrlMatchesPatternWithNoWildcards) {
  // Act & Assert
  EXPECT_TRUE(MatchUrlPattern(GURL("https://www.foo.com/"),
                              /*pattern=*/"https://www.foo.com/"));
}

TEST(BraveAdsUrlUtilTest, UrlWithPathMatchesPatternWithNoWildcards) {
  // Act & Assert
  EXPECT_TRUE(MatchUrlPattern(GURL("https://www.foo.com/bar"),
                              /*pattern=*/"https://www.foo.com/bar"));
}

TEST(BraveAdsUrlUtilTest, UrlDoesNotMatchPattern) {
  // Act & Assert
  EXPECT_FALSE(
      MatchUrlPattern(GURL("https://www.foo.com/"), /*pattern=*/"www.foo.com"));
}

TEST(BraveAdsUrlUtilTest, UrlDoesNotMatchPatternWithMissingEmptyPath) {
  // Act & Assert
  EXPECT_FALSE(MatchUrlPattern(GURL("https://www.foo.com/"),
                               /*pattern=*/"https://www.foo.com"));
}

TEST(BraveAdsUrlUtilTest, UrlMatchesEndWildcardPattern) {
  // Act & Assert
  EXPECT_TRUE(MatchUrlPattern(GURL("https://www.foo.com/bar?key=test"),
                              /*pattern=*/"https://www.foo.com/bar*"));
}

TEST(BraveAdsUrlUtilTest, UrlMatchesMidWildcardPattern) {
  // Act & Assert
  EXPECT_TRUE(MatchUrlPattern(GURL("https://www.foo.com/woo-bar-hoo"),
                              /*pattern=*/"https://www.foo.com/woo*hoo"));
}

TEST(BraveAdsUrlUtilTest, UrlDoesNotMatchMidWildcardPattern) {
  // Act & Assert
  EXPECT_FALSE(MatchUrlPattern(GURL("https://www.foo.com/woo"),
                               /*pattern=*/"https://www.foo.com/woo*hoo"));
}

TEST(BraveAdsUrlUtilTest, SameDomainOrHost) {
  // Act & Assert
  EXPECT_TRUE(SameDomainOrHost(GURL("https://foo.com?bar=test"),
                               GURL("https://subdomain.foo.com/bar")));
}

TEST(BraveAdsUrlUtilTest, NotSameDomainOrHost) {
  // Act & Assert
  EXPECT_FALSE(SameDomainOrHost(GURL("https://foo.com?bar=test"),
                                GURL("https://subdomain.bar.com/foo")));
}

TEST(BraveAdsUrlUtilTest, SameDomainOrHostForUrlWithNoSubdomain) {
  // Act & Assert
  EXPECT_TRUE(SameDomainOrHost(GURL("https://foo.com?bar=test"),
                               GURL("https://foo.com/bar")));
}

TEST(BraveAdsUrlUtilTest, NotSameDomainOrHostForUrlWithNoSubdomain) {
  // Act & Assert
  EXPECT_FALSE(SameDomainOrHost(GURL("https://foo.com?bar=test"),
                                GURL("https://bar.com/foo")));
}

TEST(BraveAdsUrlUtilTest, SameDomainOrHostForUrlWithRef) {
  // Act & Assert
  EXPECT_TRUE(SameDomainOrHost(GURL("https://foo.com?bar=test#ref"),
                               GURL("https://foo.com/bar")));
}

TEST(BraveAdsUrlUtilTest, NotSameDomainOrHostForUrlWithRef) {
  // Act & Assert
  EXPECT_FALSE(SameDomainOrHost(GURL("https://foo.com?bar=test#ref"),
                                GURL("https://bar.com/foo")));
}

TEST(BraveAdsUrlUtilTest, DomainOrHostExists) {
  // Arrange
  const std::vector<GURL> urls = {GURL("https://foo.com"),
                                  GURL("https://bar.com")};

  // Act & Assert
  EXPECT_TRUE(DomainOrHostExists(urls, GURL("https://bar.com/foo")));
}

TEST(BraveAdsUrlUtilTest, DomainOrHostDoesNotExist) {
  // Arrange
  const std::vector<GURL> urls = {GURL("https://foo.com"),
                                  GURL("https://bar.com")};

  // Act & Assert
  EXPECT_FALSE(DomainOrHostExists(urls, GURL("https://baz.com/qux")));
}

}  // namespace brave_ads
