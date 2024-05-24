/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'

// Feature-specific components
import {

  Navigation,
  // IconButton,
  // IconButtonContainer,
  // IconButtonSideText,
  IconLink,
 
} from '..'
import * as S from '../page'

// Items
import {
  // SettingsIcon,
  SettingsAdvancedIcon,
  BookmarkBook,
  HistoryIcon
} from 'brave-ui/components/icons'

// import BraveTalkIcon from './braveTalkIcon'

// Helpers
import { getLocale } from '../../../../common/locale'

export interface Props {
  textDirection: string
  // supportsBraveTalk: boolean
  backgroundImageInfo: NewTab.BackgroundWallpaper | undefined
  showPhotoInfo: boolean
  onClickSettings: () => any
}

export default class FooterInfo extends React.PureComponent<Props, {}> {
  render () {


    return (
      <>

        <S.GridItemNavigation>
          <Navigation>
            {/* <IconButtonContainer textDirection={textDirection}>
              <IconButtonSideText textDirection={textDirection}>
                <IconButton onClick={onClickSettings}>
                  <SettingsIcon />
                </IconButton>
                {getLocale('customize')}
              </IconButtonSideText>
            </IconButtonContainer> */}
            <IconLink title={getLocale('preferencesPageTitle')} href='chrome://settings'>
              <SettingsAdvancedIcon />
            </IconLink>
            <IconLink title={getLocale('bookmarksPageTitle')} href='chrome://bookmarks'>
              <BookmarkBook />
            </IconLink>
            <IconLink title={getLocale('historyPageTitle')} href='chrome://history'>
              <HistoryIcon />
            </IconLink>
            {/* { supportsBraveTalk &&
              <IconLink title={getLocale('braveTalkPromptTitle')} href='https://talk.browseweb3.com/widget'>
                <BraveTalkIcon />
              </IconLink>
            } */}
          </Navigation>
        </S.GridItemNavigation>
      </>
    )
  }
}
