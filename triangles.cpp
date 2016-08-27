#include "graphics.hpp"

//Vertices of triangle must be defined in anticlockwise order
gp::triangleStrip::triangleStrip(sf::Vector3f v1,sf::Vector3f v2,sf::Vector3f v3)
{
    origVertices[0]=v1;
    origVertices[1]=v2;
    origVertices[2]=v3;
}

void gp::triangleStrip::calcParameter()
{
    sf::Vector3f v1=origVertices[0];
    sf::Vector3f v2=origVertices[1];
    sf::Vector3f v3=origVertices[2];

    //Computes the equation of surface in world coordinate
    sf::Vector3f N=crossProduct(v2-v1,v3-v1);

    origA=N.x;
    origB=N.y;
    origC=N.z;
    origD=-1*dotProduct(N,v2);
}


void gp::triangleStrip::setWindow(gp::mainWindow *wd)
{
    window=wd;
}

sf::Vector3f gp::triangleStrip::maxY()
{
    sf::Vector3f temp=projVertices[0];
    for(int i=1; i<3; i++)
    {
        if(projVertices[i].y > temp.y)
            temp=projVertices[i];
    }
    return temp;
}

sf::Vector3f gp::triangleStrip::minY()
{
    sf::Vector3f temp=projVertices[0];
    for(int i=1; i<3; i++)
    {
        if(projVertices[i].y < temp.y)
            temp=projVertices[i];
    }
    return temp;
}

sf::Vector3f gp::triangleStrip::getEnd(sf::Vector3f pt)
{
    for(int i=0; i<3; i++)
    {
        if(projVertices[i]==pt)
        {
            if(i==2)
                return projVertices[0];
            else
                return projVertices[i+1];
        }
    }
    return pt;
}

sf::Vector3f gp::triangleStrip::getStart(sf::Vector3f pt)
{
    for(int i=0; i<3; i++)
    {
        if(projVertices[i]==pt)
        {
            if(i==0)
                return projVertices[2];
            else
                return projVertices[i-1];
        }
    }
    return pt;
}

sf::Vector3f gp::triangleStrip::getIntensity(sf::Vector3f pt)
{
    for(int i=0; i<3; i++)
    {
        if(projVertices[i]==pt)
            return intensities[i];
    }
    return sf::Vector3f(0,0,0);
}

sf::Vector3f gp::triangleStrip::getNormal(sf::Vector3f pt)
{
    for(int i=0; i<3; i++)
    {
        if(projVertices[i]==pt)
            return normals[i];
    }
    return sf::Vector3f(0,0,0);
}

//Function to project the 3D points in projection plane
void gp::triangleStrip::project()
{
    for(int i=0; i<3; i++)
        projVertices[i]=window->transformPoint(origVertices[i]);

    //Computes the equation of surface
    sf::Vector3f v1=projVertices[0],v2=projVertices[1],v3=projVertices[2];
    sf::Vector3f N=crossProduct(v2-v1,v3-v1);

    projA=N.x;
    projB=N.y;
    projC=N.z;
    projD=-1*dotProduct(N,v2);
}


void gp::triangleStrip::draw()
{
    //Project the triangle into projection plane
    project();

    //Maximum and Minimum value of y in projection plane
    sf::Vector3f localMax=maxY(),localMin=minY();

    sf::Vector3f leftStart=localMax, leftEnd=getEnd(leftStart);
    sf::Vector3f rightEnd=localMax, rightStart=getStart(rightEnd);

    //Scan-Line algorithm to fill a triangle
    do
    {
        leftEnd=getEnd(leftStart);

        for(int yscan=leftStart.y; yscan>=leftEnd.y; yscan--)
        {
            float minX,maxX;

            sf::Vector3f leftIntensity;
            float x1=leftStart.x,y1=leftStart.y;
            float x2=leftEnd.x,y2=leftEnd.y;

            minX=(yscan-y1)*(x2-x1)/(y2-y1)+x1;
            leftIntensity=(yscan-y2)/(y1-y2)*getIntensity(leftStart)+(y1-yscan)/(y1-y2)*getIntensity(leftEnd);

            if(yscan<rightStart.y)
            {
                rightEnd=rightStart;
                rightStart=getStart(rightEnd);
            }

            sf::Vector3f rightIntensity;
            x1=rightStart.x;
            y1=rightStart.y;
            x2=rightEnd.x;
            y2=rightEnd.y;

            maxX=(yscan-y1)*(x2-x1)/(y2-y1)+x1;
            rightIntensity=(yscan-y1)/(y2-y1)*getIntensity(rightEnd)+(y2-yscan)/(y2-y1)*getIntensity(rightStart);

            if(minX>maxX)
            {
                swap(minX,maxX);
                swap(leftIntensity,rightIntensity);
            }

            for(int x=minX; x<=maxX; x++)
            {
                float z=(-projA*x-projB*yscan-projD)/projC;

                sf::Vector3f pointIntensity=(maxX-x)/(maxX-minX)*leftIntensity+(x-minX)/(maxX-minX)*rightIntensity;

                gp::matrix pt(x,yscan,z,1);
                gp::matrix temp=window->invtransform*pt;

                temp=temp/temp(3,0);

                window->vertexTableArray.push(sf::Vector3f(temp(0,0),temp(1,0),temp(2,0)));//, sf::Vector3f(x,yscan,z));

                window->plotPoint(sf::Vector3f(x,yscan,z),pointIntensity);
            }
        }
        leftStart=leftEnd;
    }
    while(leftEnd!=localMin);
}
