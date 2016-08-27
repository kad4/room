#include "header.hpp"
#include "graphics.hpp"

void gp::writeToFile(vector <gp::object3d> objects)
{
    ofstream objFile("room.dat");

    vector <sf::Vector3f> Vertices;
    vector <sf::Vector2f> Textures;
    vector <sf::Vector3i> Faces;
    vector <sf::Vector3i> Map;

    gp::Material M;

    string Name;
    string TName;

    unsigned int NumVertices, NumFaces;

    for (unsigned int i=0; i<objects.size(); i++)
    {
        Vertices = objects[i].vertexTable;
        Textures = objects[i].textureTable;
        Faces = objects[i].faceTable;
        Map = objects[i].textureMap;

        M = objects[i].material;

        Name = objects[i].name;
        TName = objects[i].textureName;

        NumVertices = objects[i].NumVertices;
        NumFaces = objects[i].NumFaces;

        objFile << "Name " << Name << '\n' << TName << '\n' << '\n';
        objFile << NumVertices << '\n' << NumFaces << '\n' << '\n';

        objFile << M.ns << '\n';
        objFile << '(' << M.Ka.x << ',' << M.Ka.y << ',' << M.Ka.z << ')' << '\n';
        objFile << '(' << M.Kd.x << ',' << M.Kd.y << ',' << M.Kd.z << ')' << '\n';
        objFile << '(' << M.Ks.x << ',' << M.Ks.y << ',' << M.Ks.z << ')' << '\n' << '\n';
        //stream V
        objFile << "v" << '\n' << '[';

        for (unsigned int j=0; j<Vertices.size(); j++)
        {
            objFile << '[' << Vertices[j].x << ',' << Vertices[j].y << ',' << Vertices[j].z << ']';
            if (j < Vertices.size() - 1)
                objFile << ',';
            if (j % 4 == 0)
                objFile << '\n';
            else if (j == Vertices.size() - 1)
                objFile << '\n';
        }

        objFile << ']' << '\n' << '\n';
        //stream vt
        objFile << "vt" << '\n' << '[';

        for (unsigned int j=0; j<Textures.size(); j++)
        {
            objFile << '[' << Textures[j].x << ',' << Textures[j].y << ']';
            if (j < Textures.size() - 1)
                objFile << ',';
            if (j % 4 == 0)
                objFile << '\n';
            else if (j == Textures.size() - 1)
                objFile << '\n';
        }

        objFile << ']' << '\n' << '\n';
        //stream f
        objFile << "f" << '\n' << '[';

        for (unsigned int j=0; j<Faces.size(); j++)
        {
            objFile << '[' << Faces[j].x << ',' << Faces[j].y << ',' << Faces[j].z << ']';
            if (j < Faces.size() - 1)
                objFile << ',';
            if (j % 4 == 0)
                objFile << '\n';
            else if (j == Faces.size() - 1)
                objFile << '\n';
        }

        objFile << ']' << '\n' << '\n';
        //stream ft
        objFile << "ft" << '\n' << '[';

        for (unsigned int j=0; j<Map.size(); j++)
        {
            objFile << '[' << Map[j].x << ',' << Map[j].y << ',' << Map[j].z << ']';
            if (j < Map.size() - 1)
                objFile << ',';
            if (j % 4 == 0)
                objFile << '\n';
            else if (j == Map.size() - 1)
                objFile << '\n';
        }

        objFile << ']' << '\n' << '\n';

    }
    objFile.close();
}

vector<gp::object3d> gp::parseObj (string objName)
{
    string header, temp, name;
    string line;

    vector <sf::Vector3f> objVertices;
    vector <sf::Vector2f> objTextures;
    vector <sf::Vector3i> objFaces;
    vector <sf::Vector3i> faceTextures;

    vector <gp::object3d> objects;

    string mtlibName = "";
    ifstream objFile(objName.c_str());

    unsigned int objectCount = 0, verticesCount = 0, facesCount = 0;
    gp::object3d *tempObj = new gp::object3d();

    if (objFile.fail())
    {
        cerr << "Error";
        exit(1);
    }

    while (!objFile.eof())
    {
        getline(objFile, header, ' ');

        if (header == "#" || header == "s" || header == "g")
        {
            getline(objFile, temp);
        }
        else if (header == "mtllib")
        {
            getline(objFile, temp);
            mtlibName = temp;
        }
        else if (header == "usemtl")
        {
            getline(objFile, temp);

            ifstream mtlibFile(mtlibName.c_str());

            string mheader, mtemp = "";

            while (mtemp != temp)
            {
                getline (mtlibFile, mheader, ' ');
                while (mheader != "newmtl")
                {
                    getline(mtlibFile, mtemp);

                    getline(mtlibFile, mheader, ' ');
                }
                getline(mtlibFile, mtemp);
            }

            sf::Vector3f Ka, Kd, Ks;
            float Ns, r,g,b;
            string textureName;
            while(!mtlibFile.eof())
            {
                getline(mtlibFile, mheader, ' ');
                if (mheader == "Ns")
                {
                    mtlibFile >> Ns;
                    mtlibFile.ignore();
                }
                else if (mheader == "Ka")
                {
                    mtlibFile >> r >> g >> b;
                    mtlibFile.ignore();

                    Ka = sf::Vector3f(r,g,b);
                }
                else if (mheader == "Kd")
                {
                    mtlibFile >> r >> g >> b;
                    mtlibFile.ignore();

                    Kd = sf::Vector3f(r,g,b);
                }
                else if (mheader == "Ks")
                {
                    mtlibFile >> r >> g >> b;
                    mtlibFile.ignore();

                    Ks = sf::Vector3f(r,g,b);
                }
                else if (mheader == "map_Ka" || mheader == "map_Kd" || mheader == "map_Ks")
                {
                    getline(mtlibFile, mtemp);
                    textureName = mtemp;
                }
                else if(mheader == "newmtl")
                {
                    break;
                }
                else
                {
                    getline(mtlibFile, mtemp);
                }
            }
            mtlibFile.close();
            tempObj->setMaterial(gp::Material(Ka,Kd,Ks,Ns));
            tempObj->setTexture(textureName);

        }
        else if (header == "o")
        {
            objectCount++;

            if (objectCount != 1)
            {
                tempObj->addVertices(objVertices);
                tempObj->addFaces(objFaces, faceTextures);
                tempObj->addTextures(objTextures);

                objects.push_back(*tempObj);
            }
            delete tempObj;
            tempObj = new gp::object3d();

            objVertices.clear();
            objFaces.clear();
            objVertices.clear();
            faceTextures.clear();
            objTextures.clear();

            getline(objFile, temp);

            tempObj->setName(temp);
        }
        else if (header == "v")
        {
            verticesCount++;
            float x,y,z;

            objFile >> x >> y >> z;
            objFile.ignore();

            objVertices.push_back(sf::Vector3f(x,y,z));
        }
        else if (header == "vt")
        {
            float x,y;

            objFile >> x >> y;
            objFile.ignore();

            objTextures.push_back(sf::Vector2f(x,y));
        }
        else if(header == "f")
        {
            facesCount++;
            if (objTextures.size() == 0)
            {
                int x,y,z;

                objFile >> x >> y >> z;
                objFile.ignore();

                objFaces.push_back(sf::Vector3i(x,y,z));
            }
            else
            {
                string readNum;
                int par[6];

                getline(objFile, readNum, '/');
                par[0] = atoi(readNum.c_str());

                getline(objFile, readNum, ' ');
                par[1] = atoi(readNum.c_str());

                getline(objFile, readNum, '/');
                par[2] = atoi(readNum.c_str());

                getline(objFile, readNum, ' ');
                par[3] = atoi(readNum.c_str());

                getline(objFile, readNum, '/');
                par[4] = atoi(readNum.c_str());

                getline(objFile, readNum);
                par[5] = atoi(readNum.c_str());

                objFaces.push_back(sf::Vector3i(par[0],par[2],par[4]));
                faceTextures.push_back(sf::Vector3i(par[1],par[3],par[5]));
            }
        }
    }

    tempObj->addVertices(objVertices);
    tempObj->addFaces(objFaces, faceTextures);
    tempObj->addTextures(objTextures);

    objects.push_back(*tempObj);
    objectCount++;

    gp::object3d newObj = *tempObj;
    delete tempObj;
    objFile.close();

    ofstream counter("count.dat");
    counter << objectCount << ' ' << verticesCount << ' ' << facesCount;
    counter.close();

    writeToFile(objects);
    return objects;
}

