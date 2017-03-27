const binding = require('./build/Release/module.node');

const rotations = {
    0: 0,
    90: 1,
    180: 2,
    270: 3,
    1: 1,
    2: 2,
    3: 3
};

let started = false;
let height;
let width;

function start() {
    if (started) return;
    let r = binding.open();
    if (r != 0)
        throw new Error('lowLevelOpen() failed! Code: ' + r);
    binding.init();
    // binding.setRotation(0); Since it done by lowLevelInit();
    updateSize();
    started = true;
}

function updateSize() {
    let size = binding.getSize();
    width = (size / 1000000) >> 0;
    height = size % 1000000;
}

function convertRGB(r, g, b) {
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        throw new Error('Invalid color value!');
    }
    // China's dark magic
    return ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) & 0x001F);
}

module.exports = {
    setRotation(grad) {
        start();
        let index = rotations[grad];
        if (index === void 0)
            throw new Error('Invalid rotation! ' + grad);
        binding.setRotation(index);
        updateSize();
    },
    fillArea(x1, y1, x2, y2, color) {
        start();
        if (x1 < x2 || y1 < y2)
            throw new Error('Invalid coords!');
        binding.fill(x1, y1, x2, y2, color);
    },
    setPixel(x, y, color) {
        start();
        binding.fill(x, y, 1, 1, color);
    },
    fillScreen(color) {
        start();
        binding.fill(color);
    },
    fillAreaRGB(x1, y1, x2, y2, r, g, b) {
        start();
        if (x1 < x2 || y1 < y2)
            throw new Error('Invalid coords!');
        binding.fill(x1, y1, x2, y2, convertRGB(r, g, b));
    },
    setPixelRGB(x, y, r, g, b) {
        start();
        binding.fill(x, y, 1, 1, convertRGB(r, g, b));
    },
    fillScreenRGB(r, g, b) {
        start();
        binding.fill(convertRGB(r, g, b));
    },
    getDimensions() {
        start();
        return {
            height,
            width
        };
    },
    getHeight() {
        start();
        return height;
    },
    getWidth() {
        start();
        return width;
    },
    close() {
        if (binding.close() != 0)
            throw new Error('lowLevelClose() failed!');
    }
};
