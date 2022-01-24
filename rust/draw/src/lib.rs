pub const WHITE: image::Rgb<u8> = image::Rgb([255, 255, 255]);
pub const BLACK: image::Rgb<u8> = image::Rgb([0, 0, 0]);

pub fn clear(img: &mut image::RgbImage, rgb: &image::Rgb<u8>) {
    for (_x, _y, pixel) in img.enumerate_pixels_mut() {
        *pixel = *rgb;
    }
}

pub fn draw_line(img: &mut image::RgbImage, startx: u32, starty: u32, endx: u32, endy: u32) {
    img.put_pixel(startx, starty, BLACK);
    let mut y = starty;
    let dy = (endy - starty) / (endx - startx);

    for x in startx..endx {
        img.put_pixel(x, y, BLACK);
        y = y + dy;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn three_by_three_diag_line() {
        let mut imgbuf = image::ImageBuffer::new(3, 3);
        clear(&mut imgbuf, &WHITE);
        draw_line(&mut imgbuf, 0, 0, 3, 3);

        assert_eq!(*imgbuf.get_pixel(0, 0), BLACK);
        assert_eq!(*imgbuf.get_pixel(1, 1), BLACK);
        assert_eq!(*imgbuf.get_pixel(2, 2), BLACK);
    }
}
