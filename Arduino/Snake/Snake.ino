#include <FastLED.h>
#include <list>
using namespace std;

#define LEDS1_PIN 26
#define LEDS2_PIN 25
#define NUM_LEDS 256
#define WIDTH 32
#define HEIGHT 16

#define JOYSTICK_BUTTON_PIN 32
#define JOYSTICK_X_PIN 34
#define JOYSTICK_Y_PIN 35

#define GAME_SPEED_DELAY 200
#define SNAKE_START_LENGTH 3

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

enum Direction { NONE, UP, DOWN, LEFT, RIGHT };

CRGB SNAKE_COLOR = CRGB::Green;
CRGB FOOD_COLOR = CRGB::Blue;
CRGB BACKGROUND_COLOR = CRGB::Black;
CRGB WALL_COLOR = CRGB::Red;

class Joystick {
  int analogPinX, analogPinY;
  int buttonPin;
  int currentX, currentY;
  Direction currentDirection;

public:
  Joystick(int buttonPin, int xPin, int yPin)
      : buttonPin(buttonPin), analogPinX(xPin), analogPinY(yPin),
        currentX(0), currentY(0), currentDirection(NONE) {}

  void begin() {
    pinMode(buttonPin, INPUT_PULLUP);
  }

  void update() {
    currentX = analogRead(analogPinX);
    currentY = analogRead(analogPinY);

    if (currentX > 2500) {
      if (currentDirection != UP) currentDirection = DOWN;
    } else if (currentX < 1500) {
      if (currentDirection != DOWN) currentDirection = UP;
    } else if (currentY > 2500) {
      if (currentDirection != RIGHT) currentDirection = LEFT;
    } else if (currentY < 1500) {
      if (currentDirection != LEFT) currentDirection = RIGHT;
    } else {
      currentDirection = NONE;
    }
  }

  Direction getCurrentDirection() const { return currentDirection; }
};

Joystick joystick(JOYSTICK_BUTTON_PIN, JOYSTICK_X_PIN, JOYSTICK_Y_PIN);

int getLedIndex(int x, int y) {
  int index = x * 8;
  if (x % 2 == 0) {
    index += 7 - y;
  } else {
    index += y;
  }
  return index;
}

void setLed(int x, int y, CRGB color) {
  if (y < 8) {
    leds1[getLedIndex(31 - x, 7 - y)] = color;
  } else {
    leds2[getLedIndex(x, y - 8)] = color;
  }
  FastLED.show();
}

void clearDisplay() {
  fill_solid(leds1, NUM_LEDS, CRGB::Black);
  fill_solid(leds2, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void generateFood(int &foodX, int &foodY, const list<pair<int, int>> &snakeBody) {
  do {
    foodX = random(0, WIDTH);
    foodY = random(0, HEIGHT);
  } while (any_of(snakeBody.begin(), snakeBody.end(),
                  [&](const pair<int, int> &segment) {
                    return segment.first == foodX && segment.second == foodY;
                  }));
}

void setup() {
  FastLED.addLeds<WS2812B, LEDS1_PIN, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LEDS2_PIN, GRB>(leds2, NUM_LEDS);
  FastLED.setBrightness(50);
  clearDisplay();
  joystick.begin();
  delay(1000);
}

void loop() {
  list<pair<int, int>> snakeBody;
  int snakeLength = SNAKE_START_LENGTH;
  int snakeHeadX = WIDTH / 2;
  int snakeHeadY = HEIGHT / 2;
  Direction direction = RIGHT;
  Direction previousDirection = RIGHT;
  int foodX, foodY;
  bool gameOver = false;
  unsigned long lastMoveTime = 0;

  for (int i = 0; i < snakeLength; ++i) {
    snakeBody.push_back({snakeHeadX - i, snakeHeadY});
  }

  generateFood(foodX, foodY, snakeBody);

  for (const auto &segment : snakeBody) {
    setLed(segment.first, segment.second, SNAKE_COLOR);
  }
  setLed(foodX, foodY, FOOD_COLOR);

  while (!gameOver) {
    joystick.update();
    direction = joystick.getCurrentDirection();
    if (direction == NONE) direction = previousDirection;

    if (millis() - lastMoveTime >= GAME_SPEED_DELAY) {
      lastMoveTime = millis();

      int nextHeadX = snakeHeadX;
      int nextHeadY = snakeHeadY;

      if ((direction == LEFT && previousDirection == RIGHT) ||
          (direction == RIGHT && previousDirection == LEFT) ||
          (direction == UP && previousDirection == DOWN) ||
          (direction == DOWN && previousDirection == UP)) {
        direction = previousDirection;
      }

      switch (direction) {
        case DOWN:  nextHeadY--; break;
        case UP:    nextHeadY++; break;
        case LEFT:  nextHeadX--; break;
        case RIGHT: nextHeadX++; break;
        default: break;
      }

      if (nextHeadX < 0) nextHeadX = WIDTH - 1;
      if (nextHeadX >= WIDTH) nextHeadX = 0;
      if (nextHeadY < 0) nextHeadY = HEIGHT - 1;
      if (nextHeadY >= HEIGHT) nextHeadY = 0;

      for (const auto &segment : snakeBody) {
        if (segment.first == nextHeadX && segment.second == nextHeadY) {
          gameOver = true;
          break;
        }
      }

      if (gameOver) break;

      snakeHeadX = nextHeadX;
      snakeHeadY = nextHeadY;

      snakeBody.push_front({snakeHeadX, snakeHeadY});

      if (snakeHeadX == foodX && snakeHeadY == foodY) {
        snakeLength++;
        generateFood(foodX, foodY, snakeBody);
      } else {
        auto tail = snakeBody.back();
        snakeBody.pop_back();
        setLed(tail.first, tail.second, BACKGROUND_COLOR);
      }

      setLed(snakeHeadX, snakeHeadY, SNAKE_COLOR);
      setLed(foodX, foodY, FOOD_COLOR);

      previousDirection = direction;
    }
    delay(10);
  }

  for (int i = 0; i < 3; i++) {
    for (const auto &segment : snakeBody) {
      setLed(segment.first, segment.second, WALL_COLOR);
    }
    delay(200);
    for (const auto &segment : snakeBody) {
      setLed(segment.first, segment.second, BACKGROUND_COLOR);
    }
    delay(200);
  }

  clearDisplay();
  delay(1000);
}
