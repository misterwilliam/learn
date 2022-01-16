// Include the `draw` module, which is generated from image.proto.
// pub mod draw {
//     include!(concat!(env!("OUT_DIR"), "/draw.image.rs"));
// }
include!(concat!(env!("OUT_DIR"), "/draw.image.rs"));

// pub fn create_image() -> draw::Image {
//     let img = draw::Image::default();
//     img
// }
