#pragma once
#include "header.hpp"
#ifndef GP_HPP
#define  GP_HPP


namespace gp
{

class matrix
{
    float mat[4][4];
    sf::Vector2i order;
public:
    matrix(int =0, int =0);
    matrix(sf::Vector3f);
    matrix(float,float,float,float);
    matrix(float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float);

    float & operator () (int ,int);

    sf::Vector2i getOrder(void);
    void showElements(void);

    matrix inv();

    void operator=(matrix);
    matrix operator /(float);
    matrix operator *(matrix );
};


//Functions to manipulate vectors
float dotProduct(sf::Vector3f ,sf::Vector3f);
sf::Vector3f crossProduct(sf::Vector3f,sf::Vector3f);

sf::Vector3f unitVector(sf::Vector3f);
sf::Vector3f operator*(sf::Vector3f, sf::Vector3f);
sf::Vector3f makeVector(gp::matrix);

void display(sf::Vector3f);

class zbuffer
{
public:
    float z;
    zbuffer(float=-2000);
};


class Light
{
public:
    sf::Vector3f Color;
    sf::Vector3f Position;
};


class triangleStrip;
class object3d;
class vertextablearray
{
public:
    vertextablearray(){}
    vector<float> projX, projY, projZ;
    vector <sf::Vector3f> origVertex;

    void push(sf::Vector3f v);
    void push(sf::Vector3f v1, sf::Vector3f v2);
    void setProjection(sf::Vector3f v, unsigned i);
    void setOriginal(sf::Vector3f v, unsigned i);
    void clear();
    int getIndex(float xP, float yP, float zP);
    int getIndex(sf::Vector3f v);
};

class mainWindow:public sf::RenderWindow
{
    //Actual size of window
    int width,height;

    //Viewing coordinate origin and the point to look at
    sf::Vector3f viewPoint,lookAtPoint;

    //Maximum and minimum value of z in the viewing coordinate
    float zfront,zback;

    // u,v,n vectors for viewing-coordinate system
    sf::Vector3f uVector,vVector,nVector;

    // Position of viewing plane in z-axis
    float zviewPlane;

    //View volume for objects
    sf::Vector3f worldMin,worldMax;

    //Projection Reference Point
    sf::Vector3f projectionPoint;

    //Image buffer to store the pixels
    sf::Image windowImage;

    //Sprite and texture to draw in 2D
    sf::Texture windowTexture;
    sf::Sprite windowSprite;

    //Dynamic array for z-buffer
    zbuffer * points, * pshadow;

    //Ambient light intensity
    sf::Vector3f Ia;

    //Light source
    Light light;

    //World coordinate to projection coordinate transformation matrix
    matrix mtransform;
    matrix invtransform;


public:
    mainWindow(int,int);

    void clearWindow(void);
    void updateWindow(void);

    void setViewPoint(sf::Vector3f);
    void setLookAtPoint(sf::Vector3f);
    void setProjection(sf::Vector3f,float);
    void setViewVolume(sf::Vector3f,sf::Vector3f);
    void calcTransformation(void);

    void setAmbientLight(sf::Vector3f);
    void addLightSource(sf::Vector3f,sf::Vector3f);

    void mapShadow();
    void displayShadow();
    void plotPoint(sf::Vector3f,sf::Vector3f);
    sf::Vector3f transformPoint(sf::Vector3f);

    vertextablearray vertexTableArray;

    void saveScreenshot(void);

    friend class triangleStrip;
    friend class object3d;
};

class triangleStrip
{
    //Window to draw the points
    mainWindow * window;

    //Pointer to object
    object3d * obj;

    //Original vertex and surface equation
    sf::Vector3f origVertices[3];
    float origA,origB,origC,origD;

    //Projected vertex and surface equation
    sf::Vector3f projVertices[3];
    float projA,projB,projC,projD;

    sf::Vector3f intensities[3];
    sf::Vector3f normals[3];
public:

    triangleStrip(sf::Vector3f,sf::Vector3f,sf::Vector3f);

    sf::Vector3f minY();
    sf::Vector3f maxY();

    sf::Vector3f getEnd(sf::Vector3f);
    sf::Vector3f getStart(sf::Vector3f);

    sf::Vector3f getIntensity(sf::Vector3f);
    sf::Vector3f getNormal(sf::Vector3f);

    void setWindow(mainWindow *);
    void calcParameter();

    void project();
    void draw();
    void projectShadow();

    friend class Surface;
    friend class object3d;
};

class Material
{
public:
    Material (sf::Vector3f KA, sf::Vector3f KD, sf::Vector3f KS, float NS) : Ka(KA), Kd(KD), Ks(KS), ns(NS) {}
    Material () : Ka(sf::Vector3f(0,0,0)), Kd(sf::Vector3f(0,0,0)), Ks(sf::Vector3f(0,0,0)), ns(0) {}

    sf::Vector3f Ka, Kd, Ks;
    float ns;
};


class object3d
{
public:
    unsigned int NumVertices;
    unsigned int NumFaces;

    sf::Vector3f position;
    sf::Vector3f scale;
    sf::Vector3f uVector,vVector,wVector;

    string name;
    string textureName;

    vector <sf::Vector3f> vertexTable;
    vector <sf::Vector2f> textureTable;
    vector <sf::Vector3i> faceTable;
    vector <sf::Vector3i> textureMap;
    vector <triangleStrip> triangles;
    //static vector <triangleStrip> allTriangleStrips;

    Material material;
    mainWindow *window;

    object3d();

    void setWindow(mainWindow*);
    void setMaterial(gp::Material);
    void setTexture(string);
    void setName(string);

    void addVertex(sf::Vector3f);
    void addFace(sf::Vector3i, sf::Vector3i);
    void addTexture(sf::Vector2f);

    void addVertices(vector <sf::Vector3f>);
    void addFaces(vector <sf::Vector3i>, vector <sf::Vector3i>);
    void addTextures(vector <sf::Vector2f>);

    void setPosition(sf::Vector3f);
    void setScale(sf::Vector3f);
    void setAxis(sf::Vector3f,sf::Vector3f,sf::Vector3f);

    void calculateintensity();
    void mapTriangleStrip();

    void draw();

    void translate(float,float,float);
};


vector<gp::object3d> parseObj(string objName);
void writeToFile(vector <object3d> obj);

}
#endif // GP_HPP
