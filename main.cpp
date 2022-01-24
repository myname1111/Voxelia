#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <cmath>
#define PI 3.14159265358979323846

using namespace std;

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_POINTS);
        glVertex2d(0, 0);
    glEnd();

    glFlush();
}

class Voxel{
    public:
        int type;
        Voxel(int type){
            Voxel::type = type;
        }
        Voxel(){
        }
};

void init(){
    glClearColor(0, 0, 0, 1);
}

string binary(unsigned x) {
    // Warning: this breaks for numbers with more than 64 bits
    char buffer[64];
    char* p = buffer + 64;
    do
    {
        *--p = '0' + (x & 1);
    } while (x >>= 1);
    return string(p, buffer + 64);
}

struct xyz{
    double x;
    double y;
    double z;
} typedef xyz;

class OcTree{
    public:
        /*
        n = north
        w = west
        s = south
        e = east
        f = forward
        b = back
        v = voxel
        t = tree
        example:
        sefv = south east forward voxel
        */
        int nwbv;
        int nebv;
        int swbv;
        int sebv;
        int nwfv;
        int nefv;
        int swfv;
        int sefv;

        OcTree *nwbt;
        OcTree *swbt;
        OcTree *nebt;
        OcTree *sebt;
        OcTree *nwft;
        OcTree *swft;
        OcTree *neft;
        OcTree *seft;

        int voxel_size = 0;
        int voxel_type = -1;
        int is_root;
        int depth;

        OcTree(int nwbv, int nebv, int swbv, int sebv, int nwfv, int nefv, int swfv, int sefv){
            OcTree::nwbv = nwbv;
            OcTree::swbv = swbv;
            OcTree::nebv = nebv;
            OcTree::sebv = sebv;
            OcTree::nwfv = nwfv;
            OcTree::swfv = swfv;
            OcTree::nefv = nefv;
            OcTree::sefv = sefv;
            OcTree::is_root = 1;
            OcTree::depth = 0;
        }

        OcTree(OcTree nwbt, OcTree nebt, OcTree swbt, OcTree sebt, OcTree nwft, OcTree neft, OcTree swft, OcTree seft){
            OcTree::nwbt = &nwbt;
            OcTree::swbt = &swbt;
            OcTree::nebt = &nebt;
            OcTree::sebt = &sebt;
            OcTree::nwft = &nwft;
            OcTree::swft = &swft;
            OcTree::neft = &neft;
            OcTree::seft = &seft;
            OcTree::is_root = 0;
            OcTree::depth = nwbt.depth + 1;
        }

        OcTree(int voxel_size, int voxel_type){
            OcTree::voxel_size = voxel_size;
            OcTree::voxel_type = voxel_type;
            OcTree::is_root = 0;
            OcTree::depth = voxel_size;
        }

        int GetVoxel(string pos){
            if (OcTree::is_root){
                if (pos == "0"){
                    return OcTree::nwbv;
                }
                else if (pos == "1"){
                    return OcTree::nebv;
                }
                else if (pos == "2"){
                    return OcTree::swbv;
                }
                else if (pos == "3"){
                    return OcTree::sebv;
                }
                else if (pos == "4"){
                    return OcTree::nwfv;
                }
                else if (pos == "5"){
                    return OcTree::nefv;
                }
                else if (pos == "6"){
                    return OcTree::swfv;
                }
                else if (pos == "7"){
                    return OcTree::sefv;
                }
            }
            else {
                if (OcTree::voxel_size){
                    return OcTree::voxel_type;
                }
                else {
                    if (pos[0] == '0'){
                        return OcTree::nwbt->GetVoxel(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '1'){
                        return OcTree::nebt->GetVoxel(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '2'){
                        return OcTree::swbt->GetVoxel(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '3'){
                        return OcTree::sebt->GetVoxel(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '4'){
                        return OcTree::nwft->GetVoxel(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '5'){
                        return OcTree::neft->GetVoxel(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '6'){
                        return OcTree::swft->GetVoxel(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '7'){
                        return OcTree::seft->GetVoxel(pos.substr(1, pos.length()));
                    }
                }
            }
        }
    int GetVoxel(xyz pos){
        string x_bin = string(OcTree::depth - binary(pos.x).length() + 1, '0') + binary(pos.x);
        string y_bin = string(OcTree::depth - binary(pos.y).length() + 1, '0') + binary(pos.y);
        string z_bin = string(OcTree::depth - binary(pos.z).length() + 1, '0') + binary(pos.z);
        cout << "z:" << z_bin << " y:" << y_bin << " x:" << x_bin << endl;
        string out = "";
        for (int i = 0;i < OcTree::depth + 1;i++){
            out += to_string(stoi(string(1, z_bin[i]) + string(1, y_bin[i]) + string(1, x_bin[i]), nullptr, 2));
        }
        return OcTree::GetVoxel(out);
    }

    OcTree GetSmallestChild(string pos){
        if (OcTree::is_root){
                return OcTree(-1, -1);
            }
            else {
                if (OcTree::voxel_size){
                    return OcTree(-1, -1);
                }
                else {
                    if (pos[0] == '0'){
                        return OcTree::nwbt->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '1'){
                        return OcTree::nebt->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '2'){
                        return OcTree::swbt->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '3'){
                        return OcTree::sebt->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '4'){
                        return OcTree::nwft->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '5'){
                        return OcTree::neft->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '6'){
                        return OcTree::swft->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                    else if (pos[0] == '7'){
                        return OcTree::seft->GetSmallestChild(pos.substr(1, pos.length()));
                    }
                }
            }
    }
};

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1280, 720);

    glutCreateWindow("Voxelia");

    glutDisplayFunc(display);
    init();

    xyz ray_pos = {0, 0, 0};
    xyz ray_dir = {0, 0, 0};

    OcTree voxels = OcTree(
                           OcTree(0, 1,
                                  2, 3,

                                  4, 5,
                                  6, 7),
                           OcTree(8, 9,
                                  10, 11,

                                  12, 13,
                                  14, 15),
                           OcTree(16, 17, 18, 19, 20, 21, 22, 23),
                           OcTree(24, 25, 26, 27, 28, 29, 30, 31),
                           OcTree(32, 33, 34, 35, 36, 37, 38, 39),
                           OcTree(40, 41, 42, 43, 44, 45, 46, 47),
                           OcTree(48, 49, 50, 51, 52, 53, 54, 55),
                           OcTree(56, 57, 58, 59, 60, 61, 62, 63));
    xyz pos = {1, 0, 2};
    cout << PI << endl;

    int pos_aboslute_x = ray_pos.x;
    int pos_aboslute_y = ray_pos.y;
    int pos_aboslute_z = ray_pos.z;

    int pos_dif_x = ray_pos.x - pos_aboslute_x;
    int pos_dif_y = ray_pos.z - pos_aboslute_y;
    int pos_dif_z = ray_pos.y - pos_aboslute_z;

    int first_hor_x;
    int first_ver_x;
    int sub_hor_x;

    if (ray_dir.z < PI && 0 > ray_dir.z){
        first_hor_x = -pos_dif_y / tan(ray_dir.z);
        first_ver_x = -pos_dif_y;
        sub_hor_x = 1 / tan(ray_dir.z);
    };



    glutMainLoop();
    return 0;
}
