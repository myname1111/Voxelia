#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iterator>
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

int max_val(vector<int> arr){
    int maximum = 0;
    for (int element : arr){
        if (element > maximum){
            maximum = element;
        }
    }
    return maximum;
}

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
        OcTree GetSmallestChild(xyzi pos){
            string x_bin = string(OcTree::depth - binary(pos.x).length() + 1, '0') + binary(pos.x);
            string y_bin = string(OcTree::depth - binary(pos.y).length() + 1, '0') + binary(pos.y);
            string z_bin = string(OcTree::depth - binary(pos.z).length() + 1, '0') + binary(pos.z);
            string out = "";
            for (int i = 0;i < OcTree::depth + 1;i++){
                out += to_string(stoi(string(1, z_bin[i]) + string(1, y_bin[i]) + string(1, x_bin[i]), nullptr, 2));
            }
            return OcTree::GetSmallestChild(out);
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
    double pos[3] = {rpos.x, rpos.y, rpos.z};
    double dir[3] = {rdir.x, rdir.y, rdir.z};
    double coordx[3] = {pos[0], pos[1], pos[2]};
    double n_coordx[3];
    double coordy[3] = {pos[0], pos[1], pos[2]};
    double n_coordy[3];
    double coordz[3] = {pos[0], pos[1], pos[2]};
    double n_coordz[3];
    double distra[3] = {0, 0, 0};
    double n_distra[3];
    double distance[3]; // this is the distance travelled if I were to travel one unit in one direction in space
    double out[3] = {-1, -1, -1};
    double x[3];
    double y[3];
    double z[3];
    int hit[3] = {0, 0, 0};
    for (int i = 0;i < 3;i++){
        x[i] = dir[0] / dir[i];
        y[i] = dir[1] / dir[i];
        z[i] = dir[2] / dir[i];
        distance[i] = sqrt(x[i] * x[i] + y[i] * y[i] + z[i] * z[i]);
    }
    copy(begin(coordx), end(coordx), begin(n_coordx));
    copy(begin(coordy), end(coordy), begin(n_coordy));
    copy(begin(coordz), end(coordz), begin(n_coordz));
    n_distra[0] = distra[0];
    n_distra[1] = distra[1];
    n_distra[2] = distra[2];
    for (int i = 0;i < 10;i++){
        distra[0] = n_distra[0];
        distra[1] = n_distra[1];
        distra[2] = n_distra[2];
        copy(begin(n_coordx), end(n_coordx), begin(coordx));
        copy(begin(n_coordy), end(n_coordy), begin(coordy));
        copy(begin(n_coordz), end(n_coordz), begin(coordz));
        int octree_optimized_distancex = vox.GetSmallestChild(xyzi{coordx[0], coordx[1], coordx[2]}).voxel_size == -1 ?
        1 : pow(2, vox.GetSmallestChild(xyzi{coordx[0], coordx[1], coordx[2]}).depth);
        int octree_optimized_distancey = vox.GetSmallestChild(xyzi{coordy[0], coordy[1], coordy[2]}).voxel_size == -1 ?
        1 : pow(2, vox.GetSmallestChild(xyzi{coordy[0], coordy[1], coordy[2]}).depth);
        int octree_optimized_distancez = vox.GetSmallestChild(xyzi{coordz[0], coordz[1], coordz[2]}).voxel_size == -1 ?
        1 : pow(2, vox.GetSmallestChild(xyzi{coordz[0], coordz[1], coordz[2]}).depth);
        if (distra[0] != max_val(std::vector<int>(distra, distra + sizeof(distra) / sizeof(distra[0])))){
            n_distra[0] += distance[0] * octree_optimized_distancex;
            n_coordx[0] += x[0] * octree_optimized_distancex;
            n_coordx[1] += y[0] * octree_optimized_distancex;
            n_coordx[2] += z[0] * octree_optimized_distancex;
        }
        if (distra[1] != max_val(std::vector<int>(distra, distra + sizeof(distra) / sizeof(distra[0])))){
            n_distra[1] += distance[1] * octree_optimized_distancey;
            n_coordy[0] += x[1] * octree_optimized_distancey;
            n_coordy[1] += y[1] * octree_optimized_distancey;
            n_coordy[2] += z[1] * octree_optimized_distancey;
        }
        if (distra[2] != max_val(std::vector<int>(distra, distra + sizeof(distra) / sizeof(distra[0])))){
            n_distra[2] += distance[2] * octree_optimized_distancez;
            n_coordz[0] += x[2] * octree_optimized_distancez;
            n_coordz[1] += y[2] * octree_optimized_distancez;
            n_coordz[2] += z[2] * octree_optimized_distancez;
        }
        if (distra[0] == distra[1] && distra[1] == distra[2]){
            n_distra[0] += distance[0] * octree_optimized_distancex;
            n_coordx[0] += x[0] * octree_optimized_distancex;
            n_coordx[1] += y[0] * octree_optimized_distancex;
            n_coordx[2] += z[0] * octree_optimized_distancex;
        }
        if (
            max(
                max_val(std::vector<int>(n_coordx, n_coordx + sizeof(n_coordx) / sizeof(n_coordx[0]))),
                max(
                    max_val(std::vector<int>(n_coordy, n_coordy + sizeof(n_coordy) / sizeof(n_coordy[0]))),
                    max_val(std::vector<int>(n_coordz, n_coordz + sizeof(n_coordz) / sizeof(n_coordz[0])))
                )
            )
            > pow(2, vox.depth + 1)){
            hit[0] = max_val(std::vector<int>(n_coordx, n_coordx + sizeof(n_coordx) / sizeof(n_coordx[0]))) > pow(2, vox.depth + 1);
            hit[1] = max_val(std::vector<int>(n_coordy, n_coordy + sizeof(n_coordy) / sizeof(n_coordy[0]))) > pow(2, vox.depth + 1);
            hit[2] = max_val(std::vector<int>(n_coordz, n_coordz + sizeof(n_coordz) / sizeof(n_coordz[0]))) > pow(2, vox.depth + 1);
        }
        else {
            if (!hit[0]){
                if ((vox.GetVoxel(xyzi{coordx[0], coordx[1], coordx[2]}) == 1)){
                    hit[0] = max_val(std::vector<int>(n_coordx, n_coordx + sizeof(n_coordx) / sizeof(n_coordx[0]))) > pow(2, vox.depth + 1);
                    out[0] = out[0] != -1 && (
                                              sqrt(
                                                   out[0] * out[0] +
                                                   out[1] * out[1] +
                                                   out[2] * out[2]
                                                   ) <
                                              sqrt(
                                                  (n_coordx[0] - pos[0]) * (n_coordx[0] - pos[0]) +
                                                  (n_coordx[1] - pos[1]) * (n_coordx[1] - pos[1]) +
                                                  (n_coordx[2] - pos[2]) * (n_coordx[2] - pos[0]))) ?
                    sqrt(
                        (n_coordx[0] - pos[0]) * (n_coordx[0] - pos[0]) +
                        (n_coordx[1] - pos[1]) * (n_coordx[1] - pos[1]) +
                        (n_coordx[2] - pos[2]) * (n_coordx[2] - pos[0])) : out[0];
                }
            }
            if (!hit[1]){
                if ((vox.GetVoxel(xyzi{coordy[0], coordy[1], coordy[2]}) == 1)){
                    hit[1] = max_val(std::vector<int>(n_coordy, n_coordy + sizeof(n_coordy) / sizeof(n_coordy[0]))) > pow(2, vox.depth + 1);
                    out[1] = out[1] != -1 && (
                                              sqrt(
                                                   out[0] * out[0] +
                                                   out[1] * out[1] +
                                                   out[2] * out[2]
                                                   ) <
                                              sqrt(
                                                  (n_coordx[0] - pos[0]) * (n_coordx[0] - pos[0]) +
                                                  (n_coordx[1] - pos[1]) * (n_coordx[1] - pos[1]) +
                                                  (n_coordx[2] - pos[2]) * (n_coordx[2] - pos[0]))) ?
                    sqrt(
                        (n_coordx[0] - pos[0]) * (n_coordx[0] - pos[0]) +
                        (n_coordx[1] - pos[1]) * (n_coordx[1] - pos[1]) +
                        (n_coordx[2] - pos[2]) * (n_coordx[2] - pos[0])) : out[1];
                }
            }
            if (!hit[2]){
                if ((vox.GetVoxel(xyzi{coordz[0], coordz[1], coordz[2]}) == 1)){
                    hit[2] = max_val(std::vector<int>(n_coordz, n_coordz + sizeof(n_coordz) / sizeof(n_coordz[0]))) > pow(2, vox.depth + 1);
                    out[2] = out[2] != -1 && (
                                              sqrt(
                                                   out[0] * out[0] +
                                                   out[1] * out[1] +
                                                   out[2] * out[2]
                                                   ) <
                                              sqrt(
                                                  (n_coordx[0] - pos[0]) * (n_coordx[0] - pos[0]) +
                                                  (n_coordx[1] - pos[1]) * (n_coordx[1] - pos[1]) +
                                                  (n_coordx[2] - pos[2]) * (n_coordx[2] - pos[0]))) ?
                    sqrt(
                        (n_coordx[0] - pos[0]) * (n_coordx[0] - pos[0]) +
                        (n_coordx[1] - pos[1]) * (n_coordx[1] - pos[1]) +
                        (n_coordx[2] - pos[2]) * (n_coordx[2] - pos[0])) : out[2];
                }
            }
            if (hit[0] && hit[1] && hit[2]){
                return int_xyzi{vox.GetVoxel(xyzi{out[0], out[1], out[2]}), xyzi{out[0], out[1], out[2]}};
            }
        }
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_POINTS);
    /*
        for (int x = 0;x <= 256;x++){
            for (int y = 0;y <= 144;y++){
                ray_dir = {1, 1, 1};
                int_xyzi out = ray_trace(ray_pos, ray_dir, voxels);
                if (out.int_part){
                    glVertex2d(x / 256, y / 144);
                }
                cout << out.int_part << endl;
            }
    */
    glEnd();

    glFlush();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(256, 144);

    glutCreateWindow("Voxelia");

    glutDisplayFunc(display);
    init();

    xyzd ray_pos = {0, 0, 0};
    ray_dir = {0.1, 0.1, 0.1};
    int_xyzi out = ray_trace(ray_pos, ray_dir, voxels);

    cout << out.int_part << " " << out.xyzi_part.x <<  " " << out.xyzi_part.y <<  " " << out.xyzi_part.z << endl;

    glutMainLoop();
    return 0;
}
