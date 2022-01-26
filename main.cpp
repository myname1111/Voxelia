#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <cmath>
#define PI 3.14159265358979323846

using namespace std;

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

struct xyzi{
    int x;
    int y;
    int z;
} typedef xyzi;

struct xyzd{
    double x;
    double y;
    double z;
} typedef xyzd;

struct int_xyzi{
    int int_part;
    xyzi xyzi_part;
} typedef int_xyzi;

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
    int GetVoxel(xyzi pos){
        string x_bin = string(OcTree::depth - binary(pos.x).length() + 1, '0') + binary(pos.x);
        string y_bin = string(OcTree::depth - binary(pos.y).length() + 1, '0') + binary(pos.y);
        string z_bin = string(OcTree::depth - binary(pos.z).length() + 1, '0') + binary(pos.z);
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

xyzd ray_pos = {0.384, 0.683, 0};
xyzd ray_dir;

OcTree voxels = OcTree(
                        0, 0,
                        0, 0,

                        0, 0,
                        1, 1
                        );
xyzi pos = {1, 0, 2};

int_xyzi ray_trace(xyzd rpos, xyzd rdir, OcTree vox){
    xyzd delta = {
        rdir.x - rpos.x,
        rdir.y - rpos.y,
        rdir.z - rpos.z
        };

    xyzd dis_per_unit = {
        sqrt(1 + (delta.y / delta.x) * (delta.y / delta.x) + (delta.z / delta.x) * (delta.z / delta.x)),
        sqrt(1 + (delta.x / delta.y) * (delta.x / delta.y) + (delta.z / delta.y) * (delta.z / delta.y)),
        sqrt(1 + (delta.x / delta.z) * (delta.x / delta.z) + (delta.y / delta.z) * (delta.y / delta.z))
    };

    xyzd distance = {
        rdir.x + rdir.x * (rpos.x - (int)rpos.x),
        rdir.y + rdir.y * (rpos.y - (int)rpos.y),
        rdir.z + rdir.z * (rpos.z - (int)rpos.z)};
    xyzi coords = {rpos.x + distance.x, rpos.y + distance.y, rpos.z + distance.z};
    int out = 0;
    int voxel_get = 0; // default voxel when out of range

    for (int i = 0;!out && i < 1000;i++){
        double max_dis = max(max(distance.x, distance.y), distance.z);
        if (distance.x == max_dis && !isnan(distance.x)){
            distance.x += dis_per_unit.x;
        }
        if (distance.y == max_dis && !isnan(distance.y)){
            distance.y += dis_per_unit.y;
        }
        if (distance.z == max_dis && !isnan(distance.z)){
            distance.z += dis_per_unit.z;
        }
        xyzi n_coords = {ray_pos.x + distance.x, ray_pos.y + distance.x, ray_pos.z + distance.x};
        if (max(max(n_coords.x, n_coords.y), n_coords.z) >= pow(2, vox.depth)){
            out = 1;
        }
        else {
            coords = n_coords;
            if (vox.GetVoxel(coords) != 0){
                out = 1;
                voxel_get = vox.GetVoxel(coords);
            }
        }
    }
    return int_xyzi{voxel_get, coords};
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_POINTS);
        for (int x = 0;x <= 1366;x++){
            for (int y = 0;y <= 768;y++){
                ray_dir = {x / 100, y / 100, 0};
                int_xyzi out = ray_trace(ray_pos, ray_dir, voxels);
                if (out.int_part){
                    glVertex2d(x / 1366, y / 768);
                }
            }
        }
    glEnd();

    glFlush();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1366, 768);

    glutCreateWindow("Voxelia");

    glutDisplayFunc(display);
    init();

    glutMainLoop();
    return 0;
}
