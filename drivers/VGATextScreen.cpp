#include "VGATextScreen.hpp"


VGATextScreen::VGATextScreen() 
    : _ctrl(0x3d4)
    , _data(0x3d5)
    , _foreground(Colour::White)
    , _background(Colour::Black)
    , _cursorLocation(readCursorLocation()) {

}

void VGATextScreen::clearScreen() {
    unsigned int screenSize = maxCols * maxRows;

    char* vgaScreen = (char*) VIDEO_ADDRESS;
    char colour = colourToChar(_foreground, _background);

    for (int i = 0; i < screenSize * 2; i += 2) {
        vgaScreen[i] = ' ';
        vgaScreen[i + 1] = colour;
    }

    setCursorLocation({0, 0});
}

void VGATextScreen::print(char* message, UPoint location, Colour foreground, Colour background) {
    print(message, location, colourToChar(foreground, background));
}

void VGATextScreen::print(char *message, UPoint location, VGATextScreen::Colour foreground) {
    print(message, location, foreground, _background);
}

void VGATextScreen::print(char* message, UPoint location) {
    print(message, location, _foreground);
}

void VGATextScreen::print(char *message, VGATextScreen::Colour foreground, VGATextScreen::Colour background) {
    print(message, _cursorLocation, colourToChar(foreground, background));
}

void VGATextScreen::print(char *message, VGATextScreen::Colour foreground) {
    print(message, foreground, _background);
}

void VGATextScreen::print(char* message) {
    print(message, _cursorLocation);
}

void VGATextScreen::print(char *message, UPoint location, char colour) {
    char* vgaScreen = (char*) VIDEO_ADDRESS;
    _cursorLocation = location;

    for (char* c = message; *c != '\0'; c++) {
        char character = *c;

        switch (character) {
            case '\n':
            case '\r':
                setCursorLocation({0, _cursorLocation.y + 1});
                break;
            default:
                unsigned int offset = getMemoryOffset();
                vgaScreen[offset] = character;
                vgaScreen[offset + 1] = colour;
                setCursorLocation({_cursorLocation.x + 1, _cursorLocation.y});
                break;
        }
    }
}

UPoint VGATextScreen::getCursorLocation() const {
    return _cursorLocation;
}

void VGATextScreen::setColourScheme(VGATextScreen::Colour foreground, VGATextScreen::Colour background) {
    _foreground = foreground;
    _background = background;
}

void VGATextScreen::setCursorLocation(UPoint location) {
    _cursorLocation = location;

    unsigned int offset = location.y * maxCols + location.x;

    _ctrl.writeByte(14);
    _data.writeByte(offset >> 8);
    _ctrl.writeByte(15);
    _data.writeByte(offset & 0xff);
}

UPoint VGATextScreen::readCursorLocation() {
    _ctrl.writeByte(14);
    unsigned int offset = _data.readByte() << 8;
    _ctrl.writeByte(15);
    offset += _data.readByte();

    return UPoint(offset % maxCols, offset / maxCols);
}

unsigned int VGATextScreen::getMemoryOffset() const {
    return 2 * (_cursorLocation.y * maxCols + _cursorLocation.x);
}
