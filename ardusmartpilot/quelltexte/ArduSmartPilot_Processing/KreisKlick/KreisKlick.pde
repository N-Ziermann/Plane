float abstand;

void setup()
{
  size(400, 400);
  background(255, 255, 0);
}

void draw()
{
  if (mousePressed) {
    abstand = dist(mouseX, mouseY, 200, 200);
    if (abstand <50) {
      fill(255, 0, 0);
      ellipse(200, 200, 100, 100);
    }
    else {
      fill(0, 255, 0);
      ellipse(200, 200, 100, 100);
    }
  }
}

