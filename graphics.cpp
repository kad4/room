#include "graphics.hpp"

//vector<gp::triangleStrip> gp::object3d::allTriangleStrips;

gp::object3d::object3d()
{
    NumVertices = 0;
    NumFaces = 0;

    uVector=sf::Vector3f(1,0,0);
    vVector=sf::Vector3f(0,1,0);
    wVector=sf::Vector3f(0,0,1);
}

//Window to draw the object
void gp::object3d::setWindow(mainWindow *W)
{
    window = W;
}

void gp::object3d::setMaterial(gp::Material mat)
{
    material = mat;
}

void gp::object3d::setTexture(string n)
{
    textureName = n;
}

void gp::object3d::setName(string n)
{
    name = n;
}

void gp::object3d::addVertex(sf::Vector3f vertex)
{
    vertexTable.push_back(vertex);
    NumVertices++;
}

void gp::object3d::addFace(sf::Vector3i face, sf::Vector3i faceText)
{
    faceTable.push_back(face);

    textureMap.push_back(faceText);
    NumFaces++;
}

void gp::object3d::addTexture(sf::Vector2f texture)
{
    textureTable.push_back(texture);
}

void gp::object3d::addVertices(vector <sf::Vector3f> vertices)
{
    for (unsigned int i=0; i< vertices.size(); i++)
    {
        vertexTable.push_back(vertices[i]);
    }

    NumVertices += vertices.size();
}

void gp::object3d::addFaces(vector <sf::Vector3i> faces, vector <sf::Vector3i> textures)
{
    for (unsigned int i=0; i<faces.size(); i++)
    {
        faceTable.push_back(faces[i]);
    }
    for (unsigned int i=0; i<textures.size(); i++)
        textureMap.push_back(textures[i]);

    NumFaces += faces.size();
}

void gp::object3d::addTextures(vector <sf::Vector2f> textures)
{
    for (unsigned int i=0; i< textures.size(); i++)
        textureTable.push_back(textures[i]);
}

//Position to draw the object in world coordinate
void gp::object3d::setPosition(sf::Vector3f pos)
{
    position=pos;
}

//Axis to align the object to
void gp::object3d::setAxis(sf::Vector3f u,sf::Vector3f v,sf::Vector3f w)
{
    uVector=u;
    vVector=v;
    wVector=w;
}

void gp::object3d::setScale(sf::Vector3f s)
{
    scale=s;
}

//Converts the modeling coordinate to world coordinate
void gp::object3d::mapTriangleStrip()
{
    gp::matrix mscale(scale.x, 0,       0,       0,
                      0,       scale.y, 0,       0,
                      0,       0,       scale.z, 0,
                      0,       0,       0,       1);


    gp::matrix mtranslate(1, 0, 0, position.x,
                          0, 1, 0, position.y,
                          0, 0, 1, position.z,
                          0, 0, 0, 1);

    sf::Vector3f u=uVector;
    sf::Vector3f v=vVector;
    sf::Vector3f w=wVector;

    gp::matrix mrotate(
        u.x, u.y, u.z ,0,
        v.x, v.y, v.z, 0,
        w.x, w.y, w.z, 0,
        0,   0,   0,   1
    );

    gp::matrix mtransform=(mtranslate*(mrotate*mscale));

    triangles.clear();
    for (unsigned i = 0; i < faceTable.size(); i++)
    {
        gp::matrix vertex1(vertexTable[faceTable[i].x - 1]);
        gp::matrix temp1=mtransform*vertex1;

        gp::matrix vertex2(vertexTable[faceTable[i].y - 1]);
        gp::matrix temp2=mtransform*vertex2;

        gp::matrix vertex3(vertexTable[faceTable[i].z - 1]);
        gp::matrix temp3=mtransform*vertex3;

        triangles.push_back(gp::triangleStrip(gp::makeVector(temp1),gp::makeVector(temp2),gp::makeVector(temp3)));
        triangles[i].setWindow(window);
        triangles[i].calcParameter();

        //allTriangleStrips.push_back(triangles[i]);
    }
}

void gp::object3d::calculateintensity()
{
    //Gouraud shading models
    for(unsigned i=0; i<triangles.size(); i++)
    {
        for(unsigned j=0; j<3; j++)
        {
            sf::Vector3f N;
            for(unsigned k=0; k<triangles.size(); k++)
            {
                for(unsigned l=0; l<3; l++)
                {
                    if(triangles[i].origVertices[j]==triangles[k].origVertices[l])
                    {
                        N=N+sf::Vector3f(triangles[k].origA,triangles[k].origB,triangles[k].origC);
                    }
                }
            }

            //Average surface normal at vertex
            sf::Vector3f unitN=unitVector(N);

            //Unit vector directed towards light source
            sf::Vector3f L=unitVector(window->light.Position-triangles[i].origVertices[j]);

            //Unit vector directed towards viewer;
            sf::Vector3f V=unitVector(window->viewPoint-triangles[i].origVertices[j]);

            //Half-Way vector
            sf::Vector3f H=unitVector(L+V);

            float NL=dotProduct(unitN,L);
            float NH=dotProduct(unitN,H);

            sf::Vector3f Iamb=material.Ka*window->Ia;

            //Intensity due to ambient light and diffuse reflection
            sf::Vector3f Idiff=window->light.Color*material.Kd*NL;

            //Intensity due to specular reflection
            sf::Vector3f Ispec=window->light.Color*material.Ks*powf(fabs(NH),material.ns);

            triangles[i].intensities[j]=Iamb+Idiff+Ispec;
        }
    }
}

void gp::object3d::draw()
{
    for(unsigned i=0; i<triangles.size(); i++)
    {
        triangles[i].draw();
    }
}

void gp::object3d::translate(float tx,float ty,float tz)
{
    gp::matrix mtransform(1, 0, 0, tx,
                          0, 1, 0, ty,
                          0, 0, 1, tz,
                          0, 0, 0, 1);

    for(unsigned i=0; i<triangles.size(); i++)
    {
        gp::matrix temp;
        for(unsigned j=0; j<3; j++)
        {
            gp::matrix point(triangles[i].origVertices[j].x,
                             triangles[i].origVertices[j].y,
                             triangles[i].origVertices[j].z,
                             1);

            temp=mtransform*point;

            triangles[i].origVertices[j].x=temp(0,0);
            triangles[i].origVertices[j].y=temp(1,0);
            triangles[i].origVertices[j].z=temp(2,0);
        }
    }

    calculateintensity();
}
void gp::vertextablearray::push(sf::Vector3f v)
{
    origVertex.push_back(v);
    projX.push_back(v.x);
    projY.push_back(v.y);
    projZ.push_back(v.z);
}

void gp::vertextablearray::push(sf::Vector3f v1, sf::Vector3f v2)
{
    origVertex.push_back(v1);
    projX.push_back(v2.x);
    projY.push_back(v2.y);
    projZ.push_back(v2.z);
}

void gp::vertextablearray::setProjection(sf::Vector3f v, unsigned i)
{
    projX[i] = (v.x);
    projY[i] = (v.y);
    projZ[i] = (v.z);
}

void gp::vertextablearray::setOriginal(sf::Vector3f v, unsigned i)
{
    origVertex[i].x = v.x;
    origVertex[i].y = v.y;
    origVertex[i].z = v.z;
}

void gp::vertextablearray::clear()
{
    origVertex.clear();
    projX.clear();
    projY.clear();
    projZ.clear();
}

int gp::vertextablearray::getIndex(float xP, float yP, float zP)
{
    for (unsigned i = 0; i < origVertex.size(); i++)
        if (xP == projX[i] && yP == projY[i] && zP == projZ[i]) return i;

    return -1;
}

int gp::vertextablearray::getIndex(sf::Vector3f v)
{
    for (unsigned i = 0; i < origVertex.size(); i++)
        if (v.x == origVertex[i].x && v.y == origVertex[i].y && v.z == origVertex[i].z) return i;

    return -1;
}
