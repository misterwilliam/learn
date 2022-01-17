// Include prost generated code from draw.proto.
include!(concat!(env!("OUT_DIR"), "/draw.rs"));

// Struct Rgb is packed into an i32 in the following format.
//
// Color of a pixel packed into 32 bits. First bytes are unused, then comes
// red, green, and blue.
//
// 32      24      16       8       0
//  |-------|-------|-------|-------|
//  | blank |  red  | green | blue  |
//  |-------|-------|-------|-------|
//

const RED_MASK: i32 = 0xff0000;
const GREEN_MASK: i32 = 0x00ff00;
const BLUE_MASK: i32 = 0x0000ff;

impl Rgb {
    pub fn red(&self) -> u8 {
        let val = (self.raw_val & RED_MASK) >> 16;
        (val).try_into().unwrap()
    }
    pub fn green(&self) -> u8 {
        let val = (self.raw_val & GREEN_MASK) >> 8;
        (val).try_into().unwrap()
    }
    pub fn blue(&self) -> u8 {
        let val = self.raw_val & BLUE_MASK;
        (val).try_into().unwrap()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn getters() {
        let rgb = Rgb {raw_val: 0x123456_i32};
        assert_eq!(rgb.red(), 0x12);
        assert_eq!(rgb.green(), 0x34);
        assert_eq!(rgb.blue(), 0x56);
    }

    #[test]
    fn blah() {
        let n = 0x12i8;
        assert_eq!(n.to_be(), n);
    }
}
