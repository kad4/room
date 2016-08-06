#include "graphics.hpp"
gp::matrix::matrix(int row,int col)
{
    order=sf::Vector2i(row,col);

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            mat[i][j]=0;
}


gp::matrix::matrix(sf::Vector3f v)
{
    order=sf::Vector2i(4,1);

    for (int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            mat[i][j]=0;

    mat[0][0]=v.x;
    mat[1][0]=v.y;
    mat[2][0]=v.z;
    mat[3][0]=1;
}

gp::matrix::matrix(float a00,float a10,float a20,float a30)
{
    order=sf::Vector2i(4,1);

    for (int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            mat[i][j]=0;

    mat[0][0]=a00;
    mat[1][0]=a10;
    mat[2][0]=a20;
    mat[3][0]=a30;
}

gp::matrix::matrix(float a00,float a01,float a02,float a03,
                   float a10,float a11,float a12,float a13,
                   float a20,float a21,float a22,float a23,
                   float a30,float a31,float a32,float a33)
{
    order=sf::Vector2i(4,4);

    mat[0][0]=a00;
    mat[0][1]=a01;
    mat[0][2]=a02;
    mat[0][3]=a03;

    mat[1][0]=a10;
    mat[1][1]=a11;
    mat[1][2]=a12;
    mat[1][3]=a13;

    mat[2][0]=a20;
    mat[2][1]=a21;
    mat[2][2]=a22;
    mat[2][3]=a23;

    mat[3][0]=a30;
    mat[3][1]=a31;
    mat[3][2]=a32;
    mat[3][3]=a33;

}

sf::Vector2i gp::matrix::getOrder()
{
    return order;
}


float & gp::matrix::operator()(int x ,int y)
{
    return mat[x][y];
}


void gp::matrix::operator=(gp::matrix a)
{
    order=a.getOrder();
    for(int i=0; i<a.getOrder().x; i++)
        for(int j=0; j<a.getOrder().y; j++)
            mat[i][j]=a(i,j);
}

gp::matrix gp::matrix::operator *(gp::matrix B)
{
    gp::matrix Y(getOrder().x,B.getOrder().y);
    for(int i=0; i<getOrder().x; i++)
    {
        for(int j=0; j<B.getOrder().y; j++)
        {
            float temp=0;
            for(int k=0; k<getOrder().y; k++)
            {
                temp+=mat[i][k]*B(k,j);
            }
            Y(i,j)=temp;
        }
    }
    return Y;
}

gp::matrix gp::matrix::operator/(float num)
{
    gp::matrix Y(getOrder().x,getOrder().y);
    for(int i=0; i<order.x; i++)
    {
        for(int j=0; j<order.y; j++)
        {
            Y(i,j)=mat[i][j]/num;
        }
    }
    return Y;
}

void gp::matrix::showElements(void)
{
    for(int i=0; i<order.x; i++)
    {
        for(int j=0; j<order.y; j++)
        {
            cout<<mat[i][j]<<" ";
        }
        cout<<endl;
    }
    char x;
    cout<<"Enter any key to continue";
    cin>>x;
}

//Dot Product of two vectors
float gp::dotProduct(sf::Vector3f v1,sf::Vector3f v2)
{
    float tmp=v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
    return tmp;
}

//Cross Product of two vectors
sf::Vector3f gp::crossProduct(sf::Vector3f v1,sf::Vector3f v2)
{
    sf::Vector3f tmp;

    tmp.x=v1.y*v2.z-v1.z*v2.y;
    tmp.y=v1.z*v2.x-v1.x*v2.z;
    tmp.z=v1.x*v2.y-v1.y*v2.x;

    return tmp;
}

sf::Vector3f gp::unitVector(sf::Vector3f v)
{
    sf::Vector3f tmp;

    float magN= sqrt(v.x*v.x+v.y*v.y+v.z*v.z);

    tmp.x=v.x/magN;
    tmp.y=v.y/magN;
    tmp.z=v.z/magN;

    return tmp;
}

sf::Vector3f gp::operator*(sf::Vector3f a, sf::Vector3f b)
{
    sf::Vector3f temp;

    temp.x=a.x*b.x;
    temp.y=a.y*b.y;
    temp.z=a.z*b.z;

    return temp;
}

sf::Vector3f gp::makeVector(gp::matrix  mat)
{
    sf::Vector3f temp;

    temp.x=mat(0,0);
    temp.y=mat(1,0);
    temp.z=mat(2,0);

    return temp;
}

void gp::display(sf::Vector3f v)
{
    cout<<endl;
    cout<<"X= "<<v.x<<" Y= "<<v.y<<" Z= "<<v.z<<endl;
}
