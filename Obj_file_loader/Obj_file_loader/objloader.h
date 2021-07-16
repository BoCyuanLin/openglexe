//#include <gl/glew.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <GL/glut.h>
#include <windows.h>
#include <math.h>
#include <gl/GL.h>

// image read, save, resize library 
#define STB_IMAGE_IMPLEMENTATION
#include "package/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "package/stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "package/stb/stb_image_resize.h"
// ------------------------------------

// class: read the .obj
class ReadObjfile{
    public:
        // struct for 2D elements
        struct vec2 {
            float x = 0, y = 0;
        };
        // struct for 3D elements
        struct vec3 {
            float x = 0, y = 0, z = 0;
        };
        // struct for faces
        struct face {
            std::vector<int> vertices;
            std::vector<int> textures;
            std::vector<int> normals;
            vec3 fnormal;
        };
        // struct for computing vetex normals
        struct fnormals{
            std::vector<vec3> fnormal;
        };
        
        bool readfile(const char* filename); // read the .obj
        bool texture_loader(const char* path); // load the texture
        void object_status() const; // print the detial of the object
        void draw(int default_mode); // draw the model

        // drawing modes
        void point_mode(); // point
        void line_mode(); // line
        void solid_mode(); // solid
        // **********

        // control
        void keyboard_control(unsigned char key, int x, int y); // control keyboard
        void mouse_control(int button, int state, int x, int y); // control mouse
        // **********

        void display_texture(bool); // use texture or not
        void display_light(bool); // turn on light or not
        void rotate_deform_idle(); // idle function for deform and rotate
        void set_screenshot_path(const char *); // set the screenshot path        
        void init_glut(int argc, char* argv[], int, int, float, float); // initialize the glut


    private:
        void multiple_light(); // set light for glut
        void rescale_position(); // rescale the object by dividing maximum distance
        void rotate(); // rotate angle
        void saveImage(); // save the image use stb
        vec3 face_normal(vec3, vec3, vec3); // compute the face normal base on input three points
        vec2 UV_map(vec3); // compute UV map by formula

        std::string mpath; // store the path of object

        int mwindows_width = 500, mwindows_height = 500, mpng_channel = 4; // windows width and height and channel's number of screenshot
        bool mdis_texture = 0, mset_light = 0, mcompute_uv = 1; // use texture, light and uv map or not
        int maxis = 0, mtheta[3] = { 0, 0, 0}; // rotate parameters
        int mMode = 0; // variable to change displaying mMode
        std::string mscreenshot_path = "screenshot.png"; //  store the path of screenshot
        float mscale_obj_windows = 1; // scale the object
        int msolid_normal = 0; // variable to switch vertex normal or face normal
        float mdeform_t = 0.05, mdeform_a = acos(-1); // deforming parameters
        GLuint mtextureID; // texture ID

        // obj data
        std::vector<vec3> vertex_positions; // positions
        std::vector<vec2> vertex_textures; // textures
        std::vector<vec3> vertex_normals; // normals
        //std::vector<vec3> vertex_colors; // colors
        std::vector<face> faces; // faces with face normal, positions, textures, normals
        // **********

};

// function: compute the texture coordinate throught UV map
ReadObjfile::vec2 ReadObjfile::UV_map(vec3 v) {
    vec2 uv;
    uv.x = 0.5f + atan2(v.x, v.z) / (2 * mdeform_a);
    uv.y = 0.5f - asin(v.y) / mdeform_a;
    return uv;
}


// function: set the path of screen shot
void ReadObjfile::display_texture(bool x) {
    mdis_texture = x;
}

// function: set the light
void ReadObjfile::display_light(bool x) {
    mset_light = x;
}

// function: set the path of screen shot
void ReadObjfile::set_screenshot_path(const char * path) {
    mscreenshot_path = path;
}

// function: rescale the vertices positions
void ReadObjfile::rescale_position() {

    int i, j;   

    float max = 0, temp_num = 0, x, y, z;

    for (i = 0; i < vertex_positions.size(); i++) {
        for (j = 0; j < vertex_positions.size()-i-1; j++) {
            x = vertex_positions[i].x - vertex_positions[j].x;
            y = vertex_positions[i].y - vertex_positions[j].y;
            z = vertex_positions[i].z - vertex_positions[j].z;
            temp_num = sqrt(abs(x*x+y*y+z*z));
            if (temp_num > max) {
                max = temp_num;
            }
        }
    }

    std::cout << "The maximum of the distances: " << max << std::endl;

    //std::cout << mscale_obj_windows << max << std::endl;


    for (i = 0; i < vertex_positions.size(); i++) {
        vertex_positions[i].x = (vertex_positions[i].x / max) * mscale_obj_windows;
        vertex_positions[i].y = (vertex_positions[i].y / max) * mscale_obj_windows;
        vertex_positions[i].z = (vertex_positions[i].z / max) * mscale_obj_windows;
    }

}

// function: calculate the cross product
ReadObjfile::vec3 ReadObjfile::face_normal(vec3 t1, vec3 t2, vec3 t3) {
    
    // set points to conduct the vectors
    vec3 u, v, n;

    u.x = t2.x - t1.x;
    u.y = t2.y - t1.y;
    u.z = t2.z - t1.z;

    v.x = t3.x - t1.x;
    v.y = t3.y - t1.y;
    v.z = t3.z - t1.z;

    // cross product
    n.x = u.z * v.y - u.y * v.z;
    n.y = u.x * v.z - u.z * v.x;
    n.z = u.y * v.x - u.x * v.y;

    return n;
}


// function: read .obj file
bool ReadObjfile::readfile(const char* filename){

    mpath = filename;

    std::stringstream ss;
    std::ifstream in_file(mpath);
    std::string line = "";
    std::string prefix = "";
    vec3 temp_vec3;
    vec2 temp_vec2;
    int temp_int = 0, temp_poly_num = 0;
    face temp_face;
    

    //File open error check
    if (!in_file.is_open())
    {
        std::cout << "!!!Could not open file!!!\n";
        return 0;
    }

    //Read one line at a time
    while (std::getline(in_file, line))
    {   
        prefix = " "; // initialize the prefix

        // obtain the prefix
        ss.clear();
        ss.str(line);
        ss >> prefix;

        // case by case throught prefix
        if (prefix == "#"){
            // comment
        }else if (prefix == "v"){ // load vertices
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
            //std::cout << "R\n";
        }else if (prefix == "vt"){ //load textures
            ss >> temp_vec2.x >> temp_vec2.y;
            vertex_textures.push_back(temp_vec2);
            //std::cout << temp_vec2.x << " " << temp_vec2.y << "\n";
        }else if (prefix == "vn"){ //load normals
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            //vertex_normals.push_back(temp_vec3);
        }else if (prefix == "f"){ // load face
            int counter = 0;
            //std::cout << line << "\n";
            if (line.find("//") != std::string::npos) {
                while (ss >> temp_int)
                {
                    //std::cout << temp_int << " ";
                    //Pushing indices into correct arrays
                    //std::cout << counter << " ";
                    if (counter == 0) {
                        temp_poly_num++;
                        temp_face.vertices.push_back(temp_int);
                        //std::cout << temp_int << " ";
                    }
                    else if (counter == 1) {
                        //normal_indicies.push_back(temp_int);
                        //std::cout << temp_int << " ";
                    }
                    // handling characters
                    if (ss.peek() == '/'){
                        ++counter;
                        ss.ignore(1, '/');
                        ss.ignore(1, '/');
                    }else if (ss.peek() == ' '){
                        ++counter;
                        ss.ignore(1, ' ');
                    }
                    // initialize the counter
                    if (counter > 1) {
                        counter = 0;
                    }
                        
                }
                // triangularization the polygon
                if (temp_face.vertices.size() > 3) {
                    face temp_tri_face;
                    for (int i = 0; i < temp_face.vertices.size() - 2; i++) {
                        temp_tri_face.vertices.push_back(temp_face.vertices[0]);
                        temp_tri_face.vertices.push_back(temp_face.vertices[i + 1]);
                        temp_tri_face.vertices.push_back(temp_face.vertices[i + 2]);
                        faces.push_back(temp_tri_face);
                        temp_tri_face.vertices.clear();
                    }
                    temp_face.vertices.clear();
                }
                else {
                    faces.push_back(temp_face);
                    temp_face.vertices.clear();
                }
            }
            else if (line.find('/') != std::string::npos) {
                mcompute_uv = 0;
                while (ss >> temp_int)
                {
                    //std::cout << temp_int << " ";
                    //Pushing indices into correct arrays
                    if (counter == 0) {
                        temp_poly_num++;
                        temp_face.vertices.push_back(temp_int);
                        //std::cout << temp_int << " ";
                    }
                    else if (counter == 1) {
                        temp_face.textures.push_back(temp_int);
                        //std::cout << temp_int << " ";
                    }
                    else if (counter == 2) {
                        //normal_indicies.push_back(temp_int);
                        //std::cout << temp_int << " ";
                    }
                    // handling characters
                    if (ss.peek() == '/'){
                        ++counter;
                        ss.ignore(1, '/');
                    }else if (ss.peek() == ' '){
                        if (counter == 1) {
                            counter = 0;
                        }
                        else {
                            ++counter;
                        }
                        ss.ignore(1, ' ');
                    }
                    // initialize the counter
                    if (counter > 2) {
                        counter = 0;
                    }   
                }
                //std::cout << "\n";
                // triangularization the polygon
                if (temp_face.vertices.size() > 3) {
                    face temp_tri_face;
                    for (int i = 0; i < temp_face.vertices.size() - 2; i++) {
                        // vertixes
                        temp_tri_face.vertices.push_back(temp_face.vertices[0]);
                        temp_tri_face.vertices.push_back(temp_face.vertices[i + 1]);
                        temp_tri_face.vertices.push_back(temp_face.vertices[i + 2]);
                        // textures
                        if (!mcompute_uv) {
                            temp_tri_face.textures.push_back(temp_face.textures[0]);
                            temp_tri_face.textures.push_back(temp_face.textures[i + 1]);
                            temp_tri_face.textures.push_back(temp_face.textures[i + 2]);
                        }
                        faces.push_back(temp_tri_face);
                        temp_tri_face.vertices.clear();
                        temp_tri_face.textures.clear();
                    }

                    temp_face.vertices.clear();
                    temp_face.textures.clear();
                }
                else {
                    faces.push_back(temp_face);
                    temp_face.vertices.clear();
                }
            }
            else {
                while (ss >> temp_int){
                        temp_poly_num++;
                        temp_face.vertices.push_back(temp_int);
                        //std::cout << temp_int << " ";
                }
                // triangularization the polygon
                if (temp_face.vertices.size() > 3) {
                    face temp_tri_face;
                    for (int i = 0; i < temp_face.vertices.size() - 2; i++) {
                        // vertices 
                        temp_tri_face.vertices.push_back(temp_face.vertices[0]);
                        temp_tri_face.vertices.push_back(temp_face.vertices[i+1]);
                        temp_tri_face.vertices.push_back(temp_face.vertices[i+2]);

                        faces.push_back(temp_tri_face);
                        temp_tri_face.vertices.clear();
                    }
                    temp_face.vertices.clear();
                }
                else {
                    faces.push_back(temp_face);
                    temp_face.vertices.clear();
                }
            }
        }else{
            // except case
        }

    }

    rescale_position(); // rescale the vertices position
    
    // set color
    /*vec3 temp_color;
    temp_color.x = 1.f;
    temp_color.y = 1.f;
    temp_color.z = 1.f;*/

    std::cout << std::fixed;

    // compute the face normal*********************
    for (int i = 0; i < faces.size(); i++) {
        faces[i].fnormal = face_normal(vertex_positions[faces[i].vertices[0] - 1], vertex_positions[faces[i].vertices[1] - 1], vertex_positions[faces[i].vertices[2] - 1]);
        //std::cout << "fn" << i << ": " << faces[i].fnormal.x << " " << faces[i].fnormal.y << " " << faces[i].fnormal.z << " " << "\n";
    }
    // *****************************

    // compute the vertex normal*******************
    std::vector<fnormals> temp_normal;
    temp_normal.resize(vertex_positions.size(), fnormals());
    //std::cout << temp_normal.size() << std::endl;

    for (int i = 0; i < faces.size(); i++) {
        for (int j = 0; j < faces[i].vertices.size(); j++) {
            temp_normal[faces[i].vertices[j]-1].fnormal.push_back(faces[i].fnormal);
            //std::cout << faces[i].vertices[j] - 1 << " ";
        }
        //std::cout << "\n";
    }

    vec3 temp_vnormal;
    for (int i = 0; i < vertex_positions.size(); i++) {
        for (int j = 0; j < temp_normal[i].fnormal.size(); j++) {
            temp_vnormal.x += temp_normal[i].fnormal[j].x;
            temp_vnormal.y += temp_normal[i].fnormal[j].y;
            temp_vnormal.z += temp_normal[i].fnormal[j].z;
            //std::cout << i << " : " << temp_normal[i].fnormal[j].x << " " << temp_normal[i].fnormal[j].y << " " << temp_normal[i].fnormal[j].z << "\n";
        }
        //std::cout << temp_normal[i].fnormal.size();
        //std::cout << temp_normal[i].fnormal.size() << std::endl;
        temp_vnormal.x /= temp_normal[i].fnormal.size();
        temp_vnormal.y /= temp_normal[i].fnormal.size();
        temp_vnormal.z /= temp_normal[i].fnormal.size();
        //std::cout << " : " << temp_vnormal.x << " " << temp_vnormal.y << " " << temp_vnormal.z << "\n";
        vertex_normals.push_back(temp_vnormal);

        // initialize
        temp_vnormal.x = 0;
        temp_vnormal.y = 0;
        temp_vnormal.z = 0;
    }
    // *******************************
    
    if (mcompute_uv) {
        vertex_textures.clear();
        std::cout << "Use default UV mapping for textures!" << std::endl;
        for (int i = 0; i < vertex_positions.size(); i++) {
            vertex_textures.push_back(UV_map(vertex_positions[i]));
        }
    }

    
    //rescale_normal();// rescale the vertices normal

    std::cout << std::fixed;
    std::cout << "The path(filename): " << mpath << "\n\n";


    // summary loaded object
    std::cout << "Number of vertices: " << vertex_positions.size() << "\n";
    std::cout << "Number of textures: " << vertex_textures.size() << "\n";
    std::cout << "Number of faces: " << faces.size() << "\n";

    // load finished
    std::cout << "\nOBJ file loading finished!\n" << "\n";

    return 1;
}

// function: show the object status
void ReadObjfile::object_status() const{

    std::cout << std::fixed;
    //std::cout << "The path(filename): " << path << "\n\n";

    if (vertex_positions.size() > 0) {
        std::cout << "The vertexes: " << std::endl;
        for (unsigned int i = 0; i < vertex_positions.size(); i++) {
            std::cout << "v" << i << ": " << vertex_positions[i].x << " " << vertex_positions[i].y << " " << vertex_positions[i].z;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }else {
        std::cout << "\nThere's no vertex!\n" << std::endl;
    }

    if (vertex_textures.size() > 0) {
        std::cout << "The textures: " << std::endl;
        for (unsigned int i = 0; i < vertex_textures.size(); i++) {
            std::cout << "t" << i << ": " << vertex_textures[i].x << " " << vertex_textures[i].y;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }else {
        std::cout << "\nThere's no texture!\n" << std::endl;
    }
    if (vertex_normals.size() > 0) {
        std::cout << "The normals: " << std::endl;
        for (unsigned int i = 0; i < vertex_normals.size(); i++) {
            std::cout << "n" << i << ": " << vertex_normals[i].x << " " << vertex_normals[i].y << " " << vertex_normals[i].z;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }else {
        std::cout << "\nThere's no normal!\n" << std::endl;
    }
}

// function: points mMode
void ReadObjfile::point_mode(){



    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    rotate();
    unsigned int i;
    // display the vertex normal vector

    glBegin(GL_POINTS);
    for (i = 0; i < vertex_positions.size(); i++) {
        glVertex3f(vertex_positions[i].x, vertex_positions[i].y, vertex_positions[i].z);
    }
    glEnd();
    glPopMatrix();
    glFlush();
    
}

// function: lines mMode
void ReadObjfile::line_mode() {

    unsigned int i, j, vertices_num;

    // load vetices for faces respectively and draw lines
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    rotate();
    for (i = 0; i < faces.size(); i++) {
        vertices_num = faces[i].vertices.size() - 1;
        glBegin(GL_LINES);
        for (int j = 0; j < vertices_num; j++) {
            glVertex3f(vertex_positions[faces[i].vertices[j]-1].x, vertex_positions[faces[i].vertices[j]-1].y, vertex_positions[faces[i].vertices[j]-1].z);
            glVertex3f(vertex_positions[faces[i].vertices[j+1]-1].x, vertex_positions[faces[i].vertices[j+1]-1].y, vertex_positions[faces[i].vertices[j+1] - 1].z);
        }
        glVertex3f(vertex_positions[faces[i].vertices[vertices_num] - 1].x, vertex_positions[faces[i].vertices[vertices_num] - 1].y, vertex_positions[faces[i].vertices[vertices_num] - 1].z);
        glVertex3f(vertex_positions[faces[i].vertices[0] - 1].x, vertex_positions[faces[i].vertices[0] - 1].y, vertex_positions[faces[i].vertices[0] - 1].z);
        glEnd();
    }
    glPopMatrix();
    glFlush();
}

// function: set the light in glut
void ReadObjfile::multiple_light() {
    GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light1_diffuse[] = { 100.0, 0.0, 100.0, 100.0 };
    GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light1_position[] = { -100.0, 100.0, 1.0, 0.0 };
    GLfloat spot_direction[] = { -1.0, -1.0, 0.0 };
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
    glEnable(GL_LIGHT1);
}

// function: solid mMode
void ReadObjfile::solid_mode() {

    unsigned int i, j, vertices_num;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    rotate();

    if (mset_light) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
    }
    if (mdis_texture) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, mtextureID);
    }

    // load vetices for faces respectivelyand draw face
    for (i = 0; i < faces.size(); i++) {
        vertices_num = faces[i].vertices.size();
        glBegin(GL_TRIANGLES);
        if (msolid_normal == 0) {
                glNormal3f(faces[i].fnormal.x, faces[i].fnormal.y, faces[i].fnormal.z);
                //std::cout << faces[i].fnormal.x << faces[i].fnormal.y << faces[i].fnormal.z << "\n";
        }
        for (int j = 0; j < vertices_num; j++) {
            if (msolid_normal == 1) {
                glNormal3f(vertex_normals[faces[i].vertices[j] - 1].x, vertex_normals[faces[i].vertices[j] - 1].y, vertex_normals[faces[i].vertices[j] - 1].z);
                //std::cout << vertex_normals[faces[i].vertices[j] - 1].x << vertex_normals[faces[i].vertices[j] - 1].y << vertex_normals[faces[i].vertices[j] - 1].z << "\n";
            }
            if (mdis_texture) {
                if (!mcompute_uv) {
                    glTexCoord2f(vertex_textures[faces[i].textures[j] - 1].x, vertex_textures[faces[i].textures[j] - 1].y);
                    //std::cout << vertex_textures[faces[i].textures[j] - 1].x << vertex_textures[faces[i].textures[j] - 1].y << std::endl;
                    //std::cout << faces[i].textures[j] << faces[i].textures[j] << std::endl;
                }else {
                    glTexCoord2f(vertex_textures[faces[i].vertices[j] - 1].x, vertex_textures[faces[i].vertices[j] - 1].y);
                }
            }
            
            glVertex3f(vertex_positions[faces[i].vertices[j] - 1].x/*+sin(mdeform_a)*/, vertex_positions[faces[i].vertices[j] - 1].y/* + sin(mdeform_a)*/, vertex_positions[faces[i].vertices[j] - 1].z /*+ sin(mdeform_a)*/);
        }
        glEnd();
    }
    glPopMatrix();
    glutSwapBuffers();
    glFlush();
    
    if (mdis_texture) {
        glDisable(GL_TEXTURE_2D);
    }
    
    if (mset_light) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE);
    }
    

}

// function: rotate parameters
void ReadObjfile::rotate() {
    glRotatef(mtheta[0], 1, 0, 0);
    glRotatef(mtheta[1], 0, 1, 0);
    glRotatef(mtheta[2], 0, 0, 1);
}

// function: idle function to control the spin angle
void ReadObjfile::rotate_deform_idle() {

    Sleep(10);
    mtheta[maxis] += 2;
    if (mtheta[maxis] > 360)
        mtheta[maxis] = -360;

    mdeform_a += mdeform_t;

    glutPostRedisplay();
}


//-------------------------------------------
// transfer member function to static form
// inorder to use gldisplayfunc
ReadObjfile *disrefer;
static void call_point_mode(){disrefer->point_mode();}
static void call_line_mode(){disrefer->line_mode();}
static void call_solid_mode(){disrefer->solid_mode();}
static void call_rotate_deform_idle(){disrefer->rotate_deform_idle();}
static void call_keyboard_control(unsigned char key, int x, int y) { disrefer->keyboard_control(key, x, y);}
static void call_mouse_control(int button, int state, int x, int y) { disrefer->mouse_control(button, state, x, y); }
//-------------------------------------------

// function: use keyboard key to switch mMode
void ReadObjfile::keyboard_control(unsigned char key, int x, int y) {

    if (key == 27) {
        exit(0);
    }
    else if (key == 'S' || key == 's') {
        mMode = (mMode + 1) % 4;
        switch (mMode) {
        case 0:     // points mMode
            glutSetWindowTitle("Objloader (Points mode)");
            glutDisplayFunc(::call_point_mode);
            break;
        case 1:     // lines mMode key
            glutSetWindowTitle("Objloader (Lines mode)");
            glutDisplayFunc(::call_line_mode);
            break;
        case 2:     // solid mMode key
            glutSetWindowTitle("Objloader (Solid mode with face normals)");
            glutDisplayFunc(::call_solid_mode);
            msolid_normal = 0;
            break;
        case 3:    // solid with normals mMode
            glutSetWindowTitle("Objloader (Solid mode with vertex normals)");
            glutDisplayFunc(::call_solid_mode);
            msolid_normal++;
        }
    }
    else if (key == 'P' || key == 'p') {
        saveImage();
    }

}

// function: use mouse to switch rotating direction
void ReadObjfile::mouse_control(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        maxis = 0;
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
        maxis = 1;
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        maxis = 2;
}

// function: screen shot use package stb
void ReadObjfile::saveImage() {

    int pixels_num = mwindows_width * mwindows_height * mpng_channel; // total number of pixels

    uint8_t* pixels = new uint8_t[pixels_num];
    uint8_t* reverse_pixels = new uint8_t[pixels_num];

    glutSwapBuffers();
    glReadPixels(0, 0, mwindows_width, mwindows_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    for (int i = 0; i < mwindows_height; i++) {
        for (int j = 0; j < mwindows_width*4; j++) {
            reverse_pixels[i*mwindows_width*4+j] = pixels[(mwindows_height-i-1)*(mwindows_width*4)+j];
        }

    } 

    time_t now = time(0);

    //std::cout << "Number of sec since January 1,1970 is:: " << now << std::endl;

    tm* ltm = localtime(&now);


    std::string name = mscreenshot_path;
    name.pop_back();
    name.pop_back();
    name.pop_back();
    name.pop_back();

    name.append(std::to_string((1900 + ltm->tm_year))); // year
    name.append(std::to_string((1 + ltm->tm_mon))); // month
    name.append(std::to_string((ltm->tm_mday))); // day
    name.append(std::to_string((ltm->tm_hour))); // hour
    name.append(std::to_string((ltm->tm_min))); // minute
    name.append(std::to_string(ltm->tm_sec)); // second


    name.append(".png");

    stbi_write_png(name.c_str(), mwindows_width, mwindows_height, mpng_channel, reverse_pixels, 0);
    std::cout << "Screen Shot!!\n";   
}

bool ReadObjfile::texture_loader(const char *path) {

    std::ifstream in_file(path);
    if (!in_file.is_open())
    {
        std::cout << "!!!Could not open file!!!\n";
        return 0;
    }

    int x = 0, y = 0, n = 0;

    unsigned char* data = stbi_load(path, &x, &y, &n, 0);

    //GLuint mtextureID;
    glGenTextures(1, &mtextureID);
    glBindTexture(GL_TEXTURE_2D, mtextureID);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    gluBuild2DMipmaps(GL_TEXTURE_2D, n, x, y, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    return 1;
    //std::cout << x << " " << y << " " << n << "\n";
}

// function: draw the object
void ReadObjfile::draw(int default_mode = 0){

    disrefer = this;

    mMode = default_mode%3;

    if (mset_light) {
        multiple_light();
    }
    
    glutSetWindowTitle("Objloader (Points mMode)");

    glutDisplayFunc(::call_point_mode);
   
    glutIdleFunc(::call_rotate_deform_idle);

    glutKeyboardFunc(::call_keyboard_control);
    glutMouseFunc(::call_mouse_control);
    glutMainLoop();
    
}

// function: initialize the glut windows
void ReadObjfile::init_glut(int argc, char* argv[], int w_width, int w_height, float point_size=3.0, float line_width=1.0) {

    glutInit(&argc, argv);

    mwindows_width = w_width;
    mwindows_height = w_height;

    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(mwindows_width, mwindows_height);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Objloader(Points mMode)");

    glEnable(GL_DEPTH_TEST);
    glPointSize(point_size);
    glLineWidth(line_width);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glColor3f(1.0, 1.0, 1.0);
}
