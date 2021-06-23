#include "collision.h"
#include "skybox.h"
#include "staticmesh.h"

#include "main.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// GLFW
GLFWwindow *window;

// Scene objects
Skybox *SceneSkybox;
StaticMesh *TerrainMesh;

// Transforms
vec3  player_pos;
float player_collide_radius;
float player_gravity;

vec3  camera_orbit_rotation;

// Matrices
mat4 proj, view;
vec3 view_right, view_up, view_forward;

mat4 camera_orbit_model, player_model;

// For GLU sphere drawing
GLUquadric* sphereQuadratic;

// Light properties
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

// Global material properties
GLfloat mat_ambient[] = {0.0, 0.2, 0.4, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {64.0};

// Object-specific material properties
GLfloat mat_player_diffuse[] = {1.0, 1.0, 0.0, 1.0};
GLfloat mat_terrain_diffuse[] = {0.0, 0.5, 1.0, 1.0};

// For mouse movement
vec2 mouse_pos(0, 0);
vec2 mouse_last_pos(0, 0);
vec2 mouse_delta_pos(0, 0);

//------------------------------------------------------------
// Name: demo_init
// Desc: Perform global setup of the program
//------------------------------------------------------------
static void demo_init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Setup the projection matrix
    glMatrixMode(GL_PROJECTION);
    proj = perspective(45.0f, (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 1000.0f);
    glLoadMatrixf(&proj[0][0]);
    
    glMatrixMode(GL_MODELVIEW);
    
    // Setup our scene objects
    SceneSkybox = new Skybox();
    TerrainMesh = new StaticMesh("data/Playground.obj");
    
    // Initialize transforms
    player_pos = vec3(0, 5, 5);
    player_collide_radius = 1.0f;
    player_gravity = 0.0f;
    
    camera_orbit_rotation = vec3(0, 0, 0);
    
    // Setup GLU sphere quadratic for drawing later
    sphereQuadratic = gluNewQuadric();
    gluQuadricNormals(sphereQuadratic, GLU_SMOOTH);
    
    // HACK: Set mouse pos once so we can calculate delta pos later
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    mouse_pos = vec2(mouse_x, mouse_y);
}

//------------------------------------------------------------------
// Name: demo_resize
// Desc: GLFW callback function to handle window resize event
//------------------------------------------------------------------
static void demo_resize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    proj = perspective(45.0f, (float)width/height, 0.1f, 1000.0f);
    glLoadMatrixf(&proj[0][0]);
    
    glMatrixMode(GL_MODELVIEW);
}

//------------------------------------------------------------
// Name: window_init()
// Desc: Perform setup of GLFW and OpenGL
//------------------------------------------------------------
static bool window_init() {
    if(!glfwInit()) {
        printf("ERROR: Failed to initialize GLFW!\n");
        return false;
    }
    
    // Use OpenGL 3.0 (Last GL version with fixed-function pipeline!)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    // Enable anti-aliasing (4x MSAA)
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "sphere-triangle-collision", NULL, NULL);
    
    if(!window) {
        printf("ERROR: Failed to create GLFW window context!\n");
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    glfwSetWindowSizeCallback(window, demo_resize);
    
    // Hide and lock the mouse cursor for camera movement later
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if(glewInit() != GLEW_OK) {
        printf("ERROR: Failed to initialize GLEW!\n");
        return false;
    }
    
    return true;
}

//------------------------------------------------------------------
// Name: main
// Desc: Program entry point
//------------------------------------------------------------------
int main(int argc, char **argv) {
    if(!window_init())
        return -1;
    
    demo_init();
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Quit upon pressing ESC
        if(glfwGetKey(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        
        // Calculate mouse delta position
        mouse_last_pos = mouse_pos;
        
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        mouse_pos = vec2(mouse_x, mouse_y);
        
        mouse_delta_pos = mouse_pos - mouse_last_pos;
        
        // Spawn back at start
        if(glfwGetKey(window, GLFW_KEY_R)) {
            player_pos = vec3(0, 5, 5);
            player_gravity = 0.0f;
        }
        
        // Basic player movement
        if(glfwGetKey(window, GLFW_KEY_W))
            player_pos -= normalize(vec3(view_forward.x, 0, view_forward.z)) * 0.2f;
        else if(glfwGetKey(window, GLFW_KEY_S))
            player_pos += normalize(vec3(view_forward.x, 0, view_forward.z)) * 0.2f;
        
        if(glfwGetKey(window, GLFW_KEY_A))
            player_pos -= normalize(vec3(view_right.x, 0, view_right.z)) * 0.2f;
        else if(glfwGetKey(window, GLFW_KEY_D))
            player_pos += normalize(vec3(view_right.x, 0, view_right.z)) * 0.2f;
        
        // Jump
        if(glfwGetKey(window, GLFW_KEY_SPACE))
            player_pos.y += 0.35f;
        
        // Rotate the camera around the player using the mouse
        camera_orbit_rotation.x += mouse_delta_pos.y * 0.5f;
        camera_orbit_rotation.y += mouse_delta_pos.x * 0.5f;
        
        // Apply lazy downwards gravity
        player_pos.y += player_gravity;
        player_gravity -= 0.01f;
        
        // TODO: Maybe we could move this collision detection and response somewhere else?
        CollisionPacket collisionPacket;
        
        for(unsigned int i = 0; i < TerrainMesh->num_faces; i++) {
            // Call the collision test routine for each triangle
            bool result = IsIntersectingSphereTriangle(
                collisionPacket,
                TerrainMesh->vertices[TerrainMesh->vertex_indices[i*3]],
                TerrainMesh->vertices[TerrainMesh->vertex_indices[i*3+1]],
                TerrainMesh->vertices[TerrainMesh->vertex_indices[i*3+2]],
                player_pos,
                player_collide_radius
                );
            
            if(result) {
                // If colliding with floor or ramp, kill gravity
                if(collisionPacket.normal.y > 0.5f)
                    player_gravity = 0;
                
                // Push collision sphere away from the intersected triangle(s)
                player_pos += collisionPacket.normal * (collisionPacket.distance + 1);
            }
        }
        
        // Build the view matrix, in which the camera follows an orbital point from a distance
        camera_orbit_model = rotate(mat4(1.0f), radians(camera_orbit_rotation.x), vec3(1, 0, 0));
        camera_orbit_model = rotate(camera_orbit_model, radians(camera_orbit_rotation.y), vec3(0, 1, 0));
        camera_orbit_model = rotate(camera_orbit_model, radians(camera_orbit_rotation.z), vec3(0, 0, 1));
        camera_orbit_model = translate(camera_orbit_model, -player_pos);
        
        mat4 camera = translate(mat4(1.0f), vec3(0, 0, -10));
        
        view = camera * camera_orbit_model;
        
        // Update the view orientation vectors
        mat4 view_inverse = inverse(view);
        view_right   = normalize(vec3(view_inverse[0]));
        view_up      = normalize(vec3(view_inverse[1]));
        view_forward = normalize(vec3(view_inverse[2]));
        
        // Build the player model matrix
        player_model = translate(mat4(1.0f), player_pos);
        
        mat4 player_composite = view * player_model;
        
        // Extract the view rotation into the skybox model matrix
        mat4 skybox_model = mat4(1.0f);
        skybox_model[0] = vec4(view[0][0], view[0][1], view[0][2], 0.0f);
        skybox_model[1] = vec4(view[1][0], view[1][1], view[1][2], 0.0f);
        skybox_model[2] = vec4(view[2][0], view[2][1], view[2][2], 0.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw the skybox
        glLoadMatrixf(&skybox_model[0][0]);
        
        SceneSkybox->Draw();
        
        // Draw the player
        glLoadMatrixf(&player_composite[0][0]);
        
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_player_diffuse);
        
        gluSphere(sphereQuadratic, player_collide_radius, 20, 20);
        
        // Draw the static terrain mesh (at the world origin)
        glLoadMatrixf(&view[0][0]);
        
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_terrain_diffuse);
        
        TerrainMesh->Draw();
        
        // Frame finished
        fflush(stdout);
        glfwSwapBuffers(window);
    }
    
    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
