extern crate image;

fn main() {
    let imgx = 800;
    let imgy = 800;

    let mut imgbuf = image::ImageBuffer::new(imgx, imgy);

    for (x, y, pixel) in imgbuf.enumerate_pixels_mut() {
        let r = (0.3 * x as f32) as u8;
        let b = (0.3 * y as f32) as u8;
        *pixel = image::Rgb([r, 0, b]);
    }

    let err = imgbuf.save("gradient.png");
    match err {
        Ok(_) => println!("Saved image to gradient.png"),
        Err(error) => println!("Error saving to gradient.png: {:?}", error),
    };
}
