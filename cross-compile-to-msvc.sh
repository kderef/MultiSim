#!/usr/bin/env sh
cargo xwin build --release --target x86_64-pc-windows-msvc
echo "Exe is in ./target/x86_64-pc-windows-msvc/release"
