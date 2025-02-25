mod credentials;
mod orders;

use std::cell::RefCell;
use std::fmt;

use tracing::{event, Level};

use crate::cache::CacheNode;
use crate::models::*;
use crate::{HTTPClient, StorageClient};

#[cfg(feature = "wasm")]
const VERSION: &str = git_version!();
#[cfg(not(feature = "wasm"))]
const VERSION: &str = "unknown";

pub struct SDK<U> {
    pub client: U,
    pub environment: Environment,
    pub base_url: String,
    pub remote_sdk_url: String,
    pub cache: RefCell<CacheNode<http::Response<Vec<u8>>>>,
}

impl<U> fmt::Debug for SDK<U> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("SDK").finish()
    }
}

impl<U> SDK<U>
where
    U: HTTPClient + StorageClient,
{
    pub fn new(
        client: U,
        environment: Environment,
        base_url: Option<&str>,
        remote_sdk_url: Option<&str>,
    ) -> Self {
        let base_url = base_url.unwrap_or(match environment {
            Environment::Local => "http://localhost:3333",
            Environment::Testing => "https://payment.rewards.brave.software",
            Environment::Development => "https://payment.rewards.brave.software",
            Environment::Staging => "https://payment.rewards.bravesoftware.com",
            Environment::Production => "https://payment.rewards.browseweb3.com",
        });

        let remote_sdk_url = remote_sdk_url.unwrap_or(match environment {
            Environment::Local => "http://localhost:8080",
            Environment::Testing => "https://account.brave.software/skus",
            Environment::Development => "https://account.brave.software/skus",
            Environment::Staging => "https://account.bravesoftware.com/skus",
            Environment::Production => "https://account.browseweb3.com/skus",
        });

        SDK {
            client,
            environment,
            base_url: base_url.to_string(),
            remote_sdk_url: remote_sdk_url.to_string(),
            cache: RefCell::new(CacheNode::default()),
        }
    }

    pub async fn initialize(&self) {
        if self.environment == Environment::Local && self.client.clear().await.is_err() {
            event!(Level::ERROR, "clearing skus storage failed",);
        }
        event!(
            Level::INFO,
            environment = %self.environment,
            version = VERSION,
            "skus sdk initialized",
        );
    }
}
