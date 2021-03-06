#ifndef WINDOW_HPP
#define WINDOW_HPP

//! Window is the visible area of a graphical world.
/*!
 * The graphical world moves (horizontally, vertically, diagonally) and the window
 * shows a limited area of it.
 */
class Window: public GraphicObject2D {
private:
  Coordinate const *defaultCoordMin;
  Coordinate const *defaultCoordMax;
  double angleX;
  double angleY;
  double angleZ;

public:
  Window(vector<Coordinate*> &windowCoordinates) :
    GraphicObject2D("Window", WINDOW, windowCoordinates) {

    this->angleX = 0;
    this->angleY = 0;
    this->angleZ = 0;

    Coordinate* coordMin = windowCoordinates.front();
    Coordinate* coordMax = windowCoordinates.back();
    this->defaultCoordMin = new Coordinate(coordMin->getX(), coordMin->getY());
    this->defaultCoordMax = new Coordinate(coordMax->getX(), coordMax->getY());
  }

  ~Window() {
    delete this->defaultCoordMax;
    delete this->defaultCoordMin;
  }

  void setAngleX(int newAngle) {
    this->angleX += newAngle;
  }

  void setAngleY(int newAngle) {
    this->angleY += newAngle;
  }

  void setAngleZ(int newAngle) {
    this->angleZ += newAngle;
  }

  double getAngleX() {
    return this->angleX;
  }

  double getAngleY() {
    return this->angleY;
  }

  double getAngleZ() {
    return this->angleZ;
  }

  void zoomIn(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() + passo);
    coordMin->setY( coordMin->getY() + passo);
    coordMax->setX( coordMax->getX() - passo);
    coordMax->setY( coordMax->getY() - passo);
  }

  void zoomOut(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() - passo);
    coordMin->setY( coordMin->getY() - passo);
    coordMax->setX( coordMax->getX() + passo);
    coordMax->setY( coordMax->getY() + passo);
  }

  void goRight(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() + passo);
    coordMax->setX( coordMax->getX() + passo);
  }

  void goLeft(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() - passo);
    coordMax->setX( coordMax->getX() - passo);
  }

  void goUp(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setY( coordMin->getY() + passo);
    coordMax->setY( coordMax->getY() + passo);
  }

  void goDown(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setY( coordMin->getY() - passo);
    coordMax->setY( coordMax->getY() - passo);
  }

  void goUpLeft(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() - passo);
    coordMin->setY( coordMin->getY() + passo);
    coordMax->setX( coordMax->getX() - passo);
    coordMax->setY( coordMax->getY() + passo);
  }

  void goUpRight(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() + passo);
    coordMin->setY( coordMin->getY() + passo);
    coordMax->setX( coordMax->getX() + passo);
    coordMax->setY( coordMax->getY() + passo);
  }

  void goDownLeft(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() - passo);
    coordMin->setY( coordMin->getY() - passo);
    coordMax->setX( coordMax->getX() - passo);
    coordMax->setY( coordMax->getY() - passo);
  }

  void goDownRight(double passo) {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX( coordMin->getX() + passo);
    coordMin->setY( coordMin->getY() - passo);
    coordMax->setX( coordMax->getX() + passo);
    coordMax->setY( coordMax->getY() - passo);
  }

  void goCenter() {
    Coordinate* coordMin = getCoordinates().front();
    Coordinate* coordMax = getCoordinates().back();

    coordMin->setX(defaultCoordMin->getX());
    coordMin->setY(defaultCoordMin->getY());
    coordMax->setX(defaultCoordMax->getX());
    coordMax->setY(defaultCoordMax->getY());

    this->angleX = 0;
    this->angleY = 0;
    this->angleZ = 0;
  }

  //! Checks if 'step' for a zoom-in ist within the limits.
  /**
   * If 'step' is too big, the zoom-in operation would be inverted (doing
   * zoom-out), in that case it is considered an invalid value for 'step'.
   *
   * @param step The step of a zoom operation (@see zoomIn()).
   * @return true if 'step' doesn't exceeds the limits or false otherwise.
   */
  bool validZoomStep(double step) {
    double newXMin = getCoordinates().front()->getX() + step;
    double newXMax = getCoordinates().back()->getX() - step;
    return (newXMin < newXMax);
  }

  void computePersGeometricCenter() {
    cx = 0;
    cy = 0;
    cz = 0;
    vector<Coordinate*>::iterator it;
    for(it = this->coordinateList.begin(); it != this->coordinateList.end(); it++) {
      cx += (*it)->getXop();
      cy += (*it)->getYop();
      cz += (*it)->getZop();
    }
    cx = cx / this->coordinateList.size();
    cy = cy / this->coordinateList.size();
    cz = cz / this->coordinateList.size();
  }

};

#endif
