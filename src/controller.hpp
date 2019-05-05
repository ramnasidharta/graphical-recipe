#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <stdexcept>

#include "bezierCurve.hpp"
#include "bspline.hpp"
#include "clipping.hpp"
#include "displayFile.hpp"
#include "enum.hpp"
#include "line.hpp"
#include "objectTransformation.hpp"
#include "objDescriptor.hpp"
#include "point.hpp"
#include "polygon.hpp"
#include "view.hpp"

#define WINDOW_ORIGINAL_POSITION 10

/*! Representation of the Controller (or Control) module of the MVC (Model, View, Control) architecture */

class Controller {
private:
  View view;
  DisplayFile display;
  Clipping clipping;
  vector<Coordinate*> pointsForPolygon;
  vector<Coordinate*> pointsForCurve;

public:
  Controller() {
  }

  ~Controller() {
  }

  void run(int argc, char* argv[]) {
    view.initializeGtkWindow(argc, argv);
  }

  //! Creates an instance of a Graphical Object and asks 'view' to draw it.
  /*!
   * Uses information from 'view' to create Coordinates, instantiate a type of
   * GraphicObject and then sends this instance to 'view' to be drawn.
   */
  void createObject() {
    int currentPage = view.getCurrentPage();
    string name = view.getObjectName();
    string objType;
    GraphicObject* obj;

    switch (currentPage) {
     case POINT: {
        vector<Coordinate*> pointCoordinate = {new Coordinate(view.getEntryPointX(), view.getEntryPointY())};
        obj = new Point(name, pointCoordinate);
        objType = "PONTO";

        break;
     }
     case LINE: {
        vector<Coordinate*> lineCoordinate = {new Coordinate(view.getEntryLineX1(), view.getEntryLineY1()),
                                               new Coordinate(view.getEntryLineX2(), view.getEntryLineY2())};
        obj = new Line(name, lineCoordinate);
        objType = "LINHA";

        break;
      }
      case POLYGON: {
        try {
          if (pointsForPolygon.size() < 3) {
            throw std::runtime_error("Cannot create polygon without at least three points!");
          }
          obj = new Polygon(name, pointsForPolygon, view.getCheckBtnState());
          objType = "POLIGONO";

          pointsForPolygon.clear();
        } catch(const std::runtime_error& e) {
          std::cout << "[ERROR] " << e.what() << std::endl;
          view.logError("Pontos insuficientes para criação de polígono.\n");
        }
        break;
      }
      case CURVE: {
        try {
          if (pointsForCurve.size() < 4) {
            throw std::runtime_error("Cannot create a curve without at least 4 points!");
          }
          if (view.isCheckBtnSplineChecked()) {
            obj = new BSpline(name, pointsForCurve, view.getDelta());
          } else {
            obj = new BezierCurve(name, pointsForCurve);
          }
          objType = "CURVA";

          pointsForCurve.clear();
        } catch(const std::runtime_error& e) {
          std::cout << "[ERROR] " << e.what() << std::endl;
          view.logError("Pontos insuficientes para criação de curva.\n");
        }
        break;
      }
    }

    display.insert(obj);
    view.insertIntoListBox(*obj, objType);
    updateDrawScreen();
  }

  void createObjectsFromFile() {
    string fileName = view.chooseFile();
    ObjDescriptor od;
    vector<GraphicObject*> objs = od.read(fileName);
    for (int i = 0; i < objs.size(); ++i) {
      display.insert(objs[i]);
      showObjectIntoView(objs[i]);
    }
    updateDrawScreen();
  }

  void saveWorldToFile() {
    string fileName = view.getFileToSaveWorld();
    ObjDescriptor od;
    od.write(display.getObjs(), fileName);
    view.clearSaveWorldFile();
  }

  void showObjectIntoView(GraphicObject* gobj) {
    switch (gobj->getType()) {
      case POINT:
        view.insertIntoListBox(*gobj, "PONTO");
        break;
      case LINE:
        view.insertIntoListBox(*gobj, "LINHA");
        break;
      case POLYGON:
        view.insertIntoListBox(*gobj, "POLIGONO");
        pointsForPolygon.clear();
        break;
    }
  }

  //! Changes an object position through translation, scaling or rotation.
  /*!
   * Takes the selected object in the object list and applies a transformation.
   */
  void executeObjectTransformation() {
    int currentPage = view.getCurrentPageTransformation();
    int currentObjectIndex = view.getCurrentObjectIndex();
    GraphicObject* obj = display.getElementoNoIndice(currentObjectIndex);

    switch (currentPage) {
      case TRANSLATION: {
        Coordinate translationVector(view.getEntryTranslationX(), view.getEntryTranslationY());
        ObjectTransformation::translation(obj, &translationVector);
        break;
      }
      case SCALING: {
        Coordinate scalingVector(view.getEntryScalingX(), view.getEntryScalingY());
        ObjectTransformation::scaling(obj, &scalingVector);
        break;
      }
      case ROTATION: {
        int radioBtnChosen = view.getRotationRadioButtonState();
        double angle = view.getAngle();
        Coordinate* reference;

        if (radioBtnChosen == 1) {
          reference = new Coordinate(0,0);
        } else if (radioBtnChosen == 2) {
          Coordinate tmp = obj->getGeometricCenter();
          reference = new Coordinate(tmp.getX(), tmp.getY());
        } else {
          reference = new Coordinate(view.getEntryRotationX(), view.getEntryRotationY());
        }
        ObjectTransformation::rotation(obj, angle, reference);

        delete reference;
        break;
      }
    }

    updateDrawScreen();
  }

  void openAddObjectWindow() {
    view.openAddObjectWindow();
  }

  void openEditObjectWindow() {
    view.openEditObjectWindow();
  }

  void create_surface(GtkWidget *widget) {
    view.create_surface(widget);
  }

  void draw(cairo_t *cr) {
    view.draw(cr);
  }

  void initializeWindowViewPort() {
    view.initializeWindowViewPort();
  }

  //! Calls View::removeSelectedObject() and updates the screen with updateDrawScreen().
  void removeSelectedObject() {
    int index = view.removeFromList(view.getListObj());
    display.remove(index);
    updateDrawScreen();
  }

  void removeFromCoordPolygonList() {
    int index = view.removeFromList(view.getListCoordPolygon());
    if (index > -1) {
      pointsForPolygon.erase(pointsForPolygon.begin() + index);
    }
  }

  void removeFromCoordCurveList() {
    int index = view.removeFromList(view.getListCoordCurve());
    if (index > -1) {
      pointsForCurve.erase(pointsForCurve.begin() + index);
    }
  }

  /*!
   * Gets (x, y) from view to create a new Coordinate and then add a new line
   * to a Polygon which is being created.
   */
  void addNewLineForPolygon() {
    double x = view.getEntryPolygonX();
    double y = view.getEntryPolygonY();
    Coordinate* c = new Coordinate(x, y);
    pointsForPolygon.push_back(c);
    view.insertCoordList(view.getListCoordPolygon(), x, y);
  }

  /*!
   * Gets (x, y) from view to create a new Coordinate and then add it
   * to a Curve which is being created.
   */
  void addNewPointForCurve() {
    double x = view.getEntryCurveX();
    double y = view.getEntryCurveY();
    Coordinate* c = new Coordinate(x, y);
    pointsForCurve.push_back(c);
    view.insertCoordList(view.getListCoordCurve(), x, y);
  }

  //! Changes the visualization window (of type Window) according the op code.
  /*!
   * @param op The operation to be done on the Window (@see View::updateWindow()).
   */
  void changeWindow(int op) {
    double changeFactor;
    if (op != WINDOW_ORIGINAL_POSITION)
      changeFactor =  (op < 10) ? view.getStep() : view.getAngleRotateWindow();
    try {
      view.updateWindow(changeFactor, op);
    } catch (int e) {
      view.logWarning("Passo do zoom acima do limite!\n");
    }
    updateDrawScreen();
  }

  void updateRadioBtnState(int newState) {
    view.updateRadioBtnState(newState);
  }

  void updateClippingRadioBtnState(int newState) {
    view.updateClippingRadioBtnState(newState);
  }

  void updateCheckBtnState() {
    view.updateCheckBtnState();
  }

  void updateCheckBtnSpline() {
    view.updateCheckBtnSpline();
  }

  //! Calls 'view' to (re)drawn all elements in 'displayFile'.
  void updateDrawScreen() {
    view.clear_surface();

    // Update window coordinates
    Window* window = view.getWindow();
    double currentAngle = window->getAngle();
    Coordinate* windowCoord = window->getCoordinates().back();
    Coordinate geometriCenter = window->getGeometricCenter();
    Coordinate windowScalingFactor(1,1);
    Coordinate scalingFactor(1/windowCoord->getX(), 1/windowCoord->getY());

    view.transformSCN(window, &geometriCenter, &windowScalingFactor, currentAngle);

    Elemento<GraphicObject*>* nextElement = display.getHead();
    while (nextElement != NULL) {
      GraphicObject* element = nextElement->getInfo();
      view.transformSCN(element, &geometriCenter, &scalingFactor, currentAngle);

      switch (element->getType()) {
        case POINT:
          clipping.pointClipping(element);
          if (element->isVisible()) {
            view.transform(element);
            view.drawNewPoint(element);
          }
          break;
        case LINE:
          clipping.lineClipping(element, view.getLineClippingAlgorithm());
          if (element->isVisible()) {
            view.transform(element);
            view.drawNewLine(element);
          }
          break;
        case POLYGON: {
          clipping.polygonClipping(element);
          if (element->isVisible()) {
            view.transform(element);
            view.drawNewPolygon(element, static_cast<Polygon*>(element)->fill());
          }
          break;
        }
        case CURVE:
          clipping.curveClipping(element);
          if (element->isVisible()) {
            view.transform(element);
            view.drawNewCurve(element);
          }
        break;
      }
      nextElement = nextElement->getProximo();
    }
    view.drawViewPortArea();
  }

};

#endif
