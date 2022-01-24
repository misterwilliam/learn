extern crate image;

fn main() {
    let imgx = 10;
    let imgy = 10;

    let mut imgbuf = image::ImageBuffer::new(imgx, imgy);

    draw::clear(&mut imgbuf, &draw::WHITE);
    draw::draw_line(&mut imgbuf, 0, 0, 10, 10);

    let err = imgbuf.save("gradient.png");
    match err {
        Ok(_) => println!("Saved image to gradient.png"),
        Err(error) => println!("Error saving to gradient.png: {:?}", error),
    };
}
