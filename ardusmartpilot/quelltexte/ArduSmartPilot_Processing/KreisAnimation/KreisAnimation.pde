int x = 200;

void setup()
{
  size(400, 400);
  frameRate(2);
}

void draw()
{
  fill(255, 0, 0);
  background(255, 255, 0);
  ellipse(x, 200, 100, 100);
  x = x-1;
}

