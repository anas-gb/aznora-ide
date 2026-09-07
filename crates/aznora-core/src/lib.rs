#![forbid(unsafe_code)]

use std::path::PathBuf;

use serde::{Deserialize, Serialize};
use thiserror::Error;

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Workspace {
    pub name: String,
    pub root: PathBuf,
}

impl Workspace {
    pub fn new(name: impl Into<String>, root: impl Into<PathBuf>) -> Self {
        Self {
            name: name.into(),
            root: root.into(),
        }
    }
}

#[derive(Debug, Error)]
pub enum CoreError {
    #[error("invalid workspace name")]
    InvalidWorkspaceName,
}

pub type CoreResult<T> = Result<T, CoreError>;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn creates_workspace() {
        let workspace = Workspace::new("demo", "/tmp/demo");
        assert_eq!(workspace.name, "demo");
        assert_eq!(workspace.root, PathBuf::from("/tmp/demo"));
    }
}
