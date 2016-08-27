#include "graphics.hpp"
gp::zbuffer::zbuffer(float zval)
{
    z=zval;
}

gp::mainWindow::mainWindow(int wd,int ht): sf::RenderWindow(sf::VideoMode(wd,ht), "Project!")
{
    //Height And Width for buffers
    width=wd;
    height=ht;

    //Image buffer
    windowImage.create(width,height);
    windowTexture.create(width,height);

    //Z buffer
    points=new zbuffer [width*height];
    pshadow=new zbuffer [width*height];

}

void gp::mainWindow::clearWindow()
{
    //Clears the z-buffer
    for(int i=0; i<width; i++)
    {
        for(int j=0; j<height; j++)
        {
            points[i+width*j]=zbuffer(-2000);
            pshadow[i+width*j]=zbuffer(-2000);
        }
    }

    //Clears the intensity buffer
    windowImage.create(width,height);

}

void gp::mainWindow::mapShadow()
{
    float viewPlane=zviewPlane;
    sf::Vector3f referencePoint=projectionPoint;
    sf::Vector3f viewingOrigin=viewPoint;
    sf::Vector3f lookat=lookAtPoint;

    setViewPoint(light.Position+sf::Vector3f(0,0,10));
    setLookAtPoint(light.Position+sf::Vector3f(0,0,0));
    setProjection(light.Position+sf::Vector3f(512,384,2000),light.Position.z+500);
    setViewVolume(sf::Vector3f(0,0,400),sf::Vector3f(1024,768,-400));

    calcTransformation();

    sf::Vector3f tempProjVector;

    for (int i=0; i<vertexTableArray.origVertex.size(); i++)
    {
        tempProjVector = transformPoint(vertexTableArray.origVertex[i]);
        //cerr << vertexTableArray.origVertex.size();
        vertexTableArray.setProjection(tempProjVector, i);
//        vertexTableArray.setOriginal(origVertex[i], i);
    }

    sf::Vector3f viewPortPoint(0,0,10);

    setViewPoint(viewPortPoint);
    setLookAtPoint(sf::Vector3f(0,0,0));

    setProjection(sf::Vector3f(512,384,2000),500);
    setViewVolume(sf::Vector3f(0,0,400),sf::Vector3f(1024,768,-400));
    calcTransformation();

}


void gp::mainWindow::displayShadow()
{
    mapShadow();

    vector <sf::Vector3f> shadowPoints;

    for (unsigned i=0;i<vertexTableArray.origVertex.size();i++)
    {
        sf::Vector3f pos = sf::Vector3f(vertexTableArray.projX[i],vertexTableArray.projY[i],vertexTableArray.projZ[i]);

        int x=static_cast<int>(pos.x+0.5);
        int y=static_cast<int>(pos.y+0.5);
        float z=pos.z;

        if(pshadow[x+width*y].z<=z)
        {
            shadowPoints.push_back(sf::Vector3f(x,y,pshadow[x+width*y].z));
            plotPoint(sf::Vector3f(x,y,pshadow[x+width*y].z),sf::Vector3f(20,20,20));
            pshadow[x+width*y].z=z;
        }
        else
        {
            shadowPoints.push_back(vertexTableArray.origVertex[i]);
            plotPoint(transformPoint(vertexTableArray.origVertex[i]),sf::Vector3f(20,20,20));
        }

    }
    vertexTableArray.clear();
}

void gp::mainWindow::updateWindow()
{
    windowTexture.update(windowImage);
    windowSprite.setTexture(windowTexture);
    draw(windowSprite);
    display();
}

//Sets the viewing coordinate origin and the window size
void gp::mainWindow::setViewPoint(sf::Vector3f pt)
{
    //Storing the viewing point reference point
    viewPoint=pt;
}

//Set the point to look-at from the viewing coordinate origin
void gp::mainWindow::setLookAtPoint(sf::Vector3f pt)
{
    lookAtPoint=pt;
}

void gp::mainWindow::setProjection(sf::Vector3f prp,float point)
{
    //Projection Reference point
    projectionPoint=prp;

    //Position for projection plane
    zviewPlane=point;
}

void gp::mainWindow::setViewVolume(sf::Vector3f wmin,sf::Vector3f wmax)
{
    //Position of window in the projection plane
    worldMin=wmin;
    worldMax=wmax;

    //Maximum and minimum value for z
    zfront=worldMax.z;
    zback=worldMin.z;
}

void gp::mainWindow::setAmbientLight(sf::Vector3f col)
{
    Ia=col;
}

void gp::mainWindow::addLightSource(sf::Vector3f pt,sf::Vector3f color)
{
    //Position of light source
    light.Position=pt;

    //Color of light source
    light.Color=color;
}

void gp::mainWindow::calcTransformation()
{
    if(viewPoint==lookAtPoint)
    {
        viewPoint=sf::Vector3f(0,0,10);
        lookAtPoint=sf::Vector3f(0,0,0);
    }

    sf::Vector3f N=viewPoint-lookAtPoint;

    //nVector is the positive z direction in the viewing axis
    nVector=unitVector(N);

    //Taking V vector as (0,1,0) the direction for positive x-axis is calculated from the cross product of N and V
    sf::Vector3f VN=crossProduct(sf::Vector3f(0,1,0),N);

    //uVector is the positive x direction in viewing axis
    uVector=unitVector(VN);

    //vVector is the actual positive y direction in viewing axis
    vVector=crossProduct(nVector,uVector);
    vVector=unitVector(vVector);

    sf::Vector3f u=uVector;
    sf::Vector3f v=vVector;
    sf::Vector3f n=nVector;

    //Translation Matrix for shifting the origin
    gp::matrix t(
        1, 0, 0, -viewPoint.x,
        0, 1, 0, -viewPoint.y,
        0, 0, 1, -viewPoint.z,
        0, 0, 0, 1);

    // Rotational Matrix for rotating the axes
    gp::matrix r(
        u.x, u.y, u.z ,0,
        v.x, v.y, v.z, 0,
        n.x, n.y, n.z, 0,
        0,   0,   0,   1);

    //Composite matrix of rotational and translation matrix
    gp::matrix rt=r*t;

    float xprp=projectionPoint.x;
    float yprp=projectionPoint.y;
    float zprp=projectionPoint.z;

    float zvp=zviewPlane;

    float dp=zprp-zvp;

    float a=-1*(xprp-(worldMax.x+worldMin.x)/2.0)/zprp;
    float b=-1*(yprp-(worldMax.y+worldMin.y)/2.0)/zprp;

    //Matrix to align the frustum
    gp::matrix mshear(
        1, 0, a, -a*zprp,
        0, 1, b, -b*zprp,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    //Matrix to shear the frustum to a parallelpipe
    gp::matrix mscale(
        1, 0, -xprp/dp, xprp*zvp/dp,
        0, 1, -yprp/dp, yprp*zvp/dp,
        0, 0, 1,        0,
        0, 0, -1/dp,    zprp/dp);

    //Composite matrix for perspective projection transformation
    gp::matrix mperspective=mscale*mshear;

    //Transformation matrix
    mtransform=mperspective*rt;
    invtransform=mtransform.inv();
}

sf::Vector3f gp::mainWindow::transformPoint(sf::Vector3f pt)
{
    //World Coordinate of point
    gp::matrix wc(pt.x,
                  pt.y,
                  pt.z,
                  1);

    //Projection of the given point
    gp::matrix ppc=mtransform*wc;

    //Converting the value of h to 1
    ppc=ppc/ppc(3,0);

    // Projection of points in projection plane
    sf::Vector3f data=sf::Vector3f(ppc(0,0),ppc(1,0),ppc(2,0));
    return data;
}

void gp::mainWindow::plotPoint(sf::Vector3f pt, sf::Vector3f Intensity)
{
//    Normalizes the projection plane coordinates
//    gp::matrix ppc(pt.x,pt.y,pt.z,1);
//
//    sf::Vector3f vmin(0,0,0),vmax(1,1,1);
//
//    float Dx=(vmax.x-vmin.x)/(windowSize.x);
//    float Dy=(vmax.y-vmin.y)/(windowSize.y);
//    float Dz=(vmax.z-vmin.z)/(zfront-zback);
//
//    float Kx=vmin.x-windowPosition.x*Dx;
//    float Ky=vmin.y-windowPosition.y*Dy;
//    float Kz=vmin.z-zfront*Dz;
//
//    //Matrix to normalise the view volume
//    gp::matrix normalize(
//        Dx, 0,  0,  Kx,
//        0,  Dy, 0,  Ky,
//        0,  0,  Dz, Kz,
//        0,  0,  0,  1
//    );
//
//    //Normalized co-ordinates
//    gp::matrix npcoordinate(4,1);
//    npcoordinate=normalize*ppc;
//
//    sf::Vector3f pos;
//    pos.x=width*npcoordinate(0,0);
//    pos.y=height*(1-npcoordinate(1,0));
//    pos.z=npcoordinate(2,0);

    if(pt.z>zviewPlane)
        ;//return;

    sf::Vector3f pos;
    pos.x=pt.x;
    pos.y=height-pt.y;
    pos.z=pt.z;

    if(Intensity.x>255)
        Intensity.x=255;
    else if(Intensity.x<0)
        Intensity.x=0;
    if(Intensity.y>255)
        Intensity.y=255;
    else if(Intensity.y<0)
        Intensity.y=0;
    if(Intensity.z>255)
        Intensity.z=255;
    else if(Intensity.z<0)
        Intensity.z=0;

    sf::Color color(Intensity.x,Intensity.y,Intensity.z);

    int x=static_cast<int>(pos.x+0.5);
    int y=static_cast<int>(pos.y+0.5);
    float z=pos.z;

    if(x<0 || x>=width || y<0 || y>=height)
        return;

    if(points[x+width*y].z<=z)
    {
        points[x+width*y].z=z;
        windowImage.setPixel(x,y,color);
    }
}

void gp::mainWindow::saveScreenshot(void)
{
    int count = 0;
    string name,strCount;
    srand(time(0));
    count = 10000 + rand()%1000000;
    stringstream holyiness;
    holyiness << count;
    holyiness >> strCount;
    name = "Screenshot" + strCount + ".jpg";
    windowImage.saveToFile(name.c_str());
}
