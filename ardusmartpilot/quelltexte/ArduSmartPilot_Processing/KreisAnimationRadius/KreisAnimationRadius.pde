int x = 200;

void setup()
{
  size(400, 400);
  frameRate(20);
}

void draw()
{
  background(255, 255, 0);
  fill(255, 0, 0);
  ellipse(x, 200, 200-x, 200-x);
  x = x-1;
}

