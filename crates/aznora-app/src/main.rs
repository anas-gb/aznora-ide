use std::env;

use aznora_core::Workspace;

fn main() {
    let current_dir = env::current_dir().expect("failed to determine current directory");
    let workspace = Workspace::new("Aznora", current_dir);

    println!("Aznora IDE");
    println!("workspace: {}", workspace.name);
    println!("root: {}", workspace.root.display());
}
