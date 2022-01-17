use std::io::Result;

fn main() -> Result<()> {
    prost_build::compile_protos(&["protos/draw.proto"], &["protos/"])?;
    Ok(())
}
