#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <cmath>

using namespace std;

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_POINTS);
        glVertex2f(0, 0);
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

struct xyz{
    int x;
    int y;
    int z;
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
    string get(xyz pos){
        int size_tree = pow(2, OcTree::depth);
        string after = "";
        int out = 0;
        if (pos.x >= size_tree){
            // east
            out += 2;
        }
        if (pos.y >= size_tree){
            // north
            out += 1;
        }
        if (pos.z >= size_tree){
            // forwards
            out += 4;
        }
        if (OcTree::depth > 0){
            xyz pos_after = {pos.x % 2, pos.y % 2, pos.z % 2};
            OcTree get_tree = OcTree(-1, -1);
            if (out == 0){
                get_tree = *(OcTree::nwbt);
            }
            else if (out == 1){
                get_tree = *(OcTree::nebt);
            }
            else if (out == 2){
                get_tree = *(OcTree::swbt);
            }
            else if (out == 3){
                get_tree = *(OcTree::sebt);
            }
            else if (out == 4){
                get_tree = *(OcTree::nwft);
            }
            else if (out == 5){
                get_tree = *(OcTree::neft);
            }
            else if (out == 6){
                get_tree = *(OcTree::swft);
            }
            else if (out == 7){
                get_tree = *(OcTree::seft);
            }
            after = get_tree.get(pos_after);
        }
        return after + to_string(out);
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
                           OcTree(8, 9, 10, 11, 12, 13, 14, 15),
                           OcTree(16, 17, 18, 19, 20, 21, 22, 23),
                           OcTree(24, 25, 26, 27, 28, 29, 30, 31),
                           OcTree(32, 33, 34, 35, 36, 37, 38, 39),
                           OcTree(40, 41, 42, 43, 44, 45, 46, 47),
                           OcTree(48, 49, 50, 51, 52, 53, 54, 55),
                           OcTree(56, 57, 58, 59, 60, 61, 62, 63));
    xyz pos = {0, 0, 1}; // TODO:fix this
    cout << voxels.GetVoxel(voxels.get(pos)) << endl;

    glutMainLoop();
    return 0;
}
