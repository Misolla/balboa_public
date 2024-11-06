#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

Image3 hw_2_1(const std::vector<std::string> &params) {
      // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Real a = Real(img.width) / Real(img.height);
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    Vector3 background = Vector3{0.5, 0.5, 0.5};
    const int samples = 4;
    const Real step = Real(1) / samples;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = background;
        }
    }

    //Check for near plan clipping
    if (-p0.z < z_near || -p1.z < z_near || -p2.z < z_near) {
        return img;
    }

    //lamda function for perspective projection
    auto project = [&](Vector3 &p) -> Vector2 {
        return {-p.x / p.z, -p.y / p.z};
    };

    Vector2 p0_proj = project(p0);
    Vector2 p1_proj = project(p1);
    Vector2 p2_proj = project(p2);

    auto to_screen = [&](Vector2 &p) -> Vector2 {
        Real x_screen = img.width * (p.x + s * a) / (2 * s * a);
        Real y_screen = img.height * (s - p.y) / (2 * s);
        return {x_screen, y_screen};
    };

    p0_proj = to_screen(p0_proj);
    p1_proj = to_screen(p1_proj);
    p2_proj = to_screen(p2_proj);

    Vector2 v01 = p1_proj - p0_proj;
    Vector2 v12 = p2_proj - p1_proj;
    Vector2 v20 = p0_proj - p2_proj;
    Vector2 n01 = {v01.y, -v01.x};
    Vector2 n12 = {v12.y, -v12.x};
    Vector2 n20 = {v20.y, -v20.x};

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 antialiasing_color = {0.0, 0.0, 0.0};
            for (int i = 0; i < samples; i++) {
                for (int j = 0; j < samples; j++) {
                    Real sample_x = x + (j + 0.5) * step;
                    Real sample_y = y + (i + 0.5) * step;
                    Vector2 q = { sample_x, sample_y};
                    Real d01 = dot((q - p0_proj), n01);
                    Real d12 = dot((q - p1_proj), n12);
                    Real d20 = dot((q - p2_proj), n20);
                    if ((d01 >= 0 && d12 >= 0 && d20 >= 0) || (d01 <= 0 && d12 <= 0 && d20 <= 0)){
                        antialiasing_color += color;
                    } else {
                        antialiasing_color += background;
                    }
                }
            }
            img(x, y) = antialiasing_color / Real(samples * samples);
        }
    }
    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);
    const int samples = 4;
    const Real step = Real(1) / samples;
    Vector3 background = Vector3{0.5, 0.5, 0.5};
    std::vector<std::vector<Real>> z_buffer((img.width * samples), std::vector<Real>((img.height * samples), std::numeric_limits<Real>::infinity()));
    std::vector<std::vector<Vector3>> color_buffer((img.width * samples), std::vector<Vector3>((img.height * samples), background));
    
    Real s = 1; // scaling factor of the view frustrum
    Real a = Real(img.width) / img.height;
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = background;
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    auto project = [&](Vector3 &p) -> Vector2 {
        return {-p.x / p.z, -p.y / p.z};
    };

    auto to_screen = [&](Vector2 &p) -> Vector2 {
        Real x_screen = img.width * (p.x + s * a) / (2.0 * s * a);
        Real y_screen = img.height * (s - p.y) / (2.0  * s);
        return {x_screen, y_screen};
    };
    for (int f = 0; f < mesh.faces.size(); f++) {
        Vector3 face = mesh.faces[f];
        Vector3 color = mesh.face_colors[f];

        Vector3 p0 = mesh.vertices[face[0]];
        Vector3 p1 = mesh.vertices[face[1]];
        Vector3 p2 = mesh.vertices[face[2]];
        
        Vector2 p0_proj = project(p0);
        Vector2 p1_proj = project(p1);
        Vector2 p2_proj = project(p2);
    
        p0_proj = to_screen(p0_proj);
        p1_proj = to_screen(p1_proj);
        p2_proj = to_screen(p2_proj);

        // using floor and ceil to aligned with pixel grid
        int min_x = std::max(0, (int)std::floor(std::min({p0_proj.x, p1_proj.x, p2_proj.x})));
        int max_x = std::min(img.width - 1, (int)std::ceil(std::max({p0_proj.x, p1_proj.x, p2_proj.x})));
        int min_y = std::max(0, (int)std::floor(std::min({p0_proj.y, p1_proj.y, p2_proj.y})));
        int max_y = std::min(img.height - 1, (int)std::ceil(std::max({p0_proj.y, p1_proj.y, p2_proj.y})));

        Real area_proj = 0.5 * cross(p1_proj - p0_proj, p2_proj - p0_proj); //adding cross definition for (Vector2, Vector2)

        for (int y = min_y; y < max_y + 1; y++) {
            for (int x = min_x; x < max_x + 1; x++) {
                Vector3 antialiasing_color = {0.0, 0.0, 0.0};
                bool in_triangle = false;
                for (int i = 0; i < samples; i++) {
                    for (int j = 0; j < samples; j++) {
                        Real sample_x = x + (j + 0.5) * step;
                        Real sample_y = y + (i + 0.5) * step;
                        Vector2 p_proj = {sample_x, sample_y};
                        Real b0_proj = (0.5 * cross(p2_proj - p1_proj, p_proj - p1_proj)) / area_proj;
                        Real b1_proj = (0.5 * cross(p0_proj - p2_proj, p_proj - p2_proj)) / area_proj;
                        Real b2_proj = 1 - b1_proj - b0_proj;
                        if ( b0_proj >= 0 && b1_proj >= 0 && b2_proj >= 0) {
                            Real B = 1 / ( b0_proj / (p0.z) + b1_proj / (p1.z) + b2_proj / (p2.z) );
                            Real b0 = (b0_proj * B) / p0.z;
                            Real b1 = (b1_proj * B) / p1.z;
                            Real b2 = (b2_proj * B) / p2.z;
                            Real depth = b0 * p0.z + b1 * p1.z + b2 * p2.z;
                            Real z_buffer_x = x * samples + j;
                            Real z_buffer_y = y * samples + i;
                            if (abs(depth) < z_buffer[z_buffer_x][z_buffer_y]) {
                                z_buffer[z_buffer_x][z_buffer_y] = abs(depth);
                                color_buffer[z_buffer_x][z_buffer_y] = color;
                            } 
                            in_triangle = true;
                        } 
                    }
                }
                if (in_triangle) {
                    for (int i = 0; i < samples; i++) {
                        for (int j = 0; j < samples; j++) {
                            Real z_buffer_x = x * samples + j;
                            Real z_buffer_y = y * samples + i;
                            antialiasing_color += color_buffer[z_buffer_x][z_buffer_y];
                        }
                    }
                    img(x, y) = antialiasing_color / Real(samples * samples);
                }
            }
            
        }

    }

    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);
    const int samples = 4;
    const Real step = Real(1) / samples;
    Vector3 background = Vector3{0.5, 0.5, 0.5};
    std::vector<std::vector<Real>> z_buffer((img.width * samples), std::vector<Real>((img.height * samples), std::numeric_limits<Real>::infinity()));
    std::vector<std::vector<Vector3>> color_buffer((img.width * samples), std::vector<Vector3>((img.height * samples), background));
    
    Real s = 1; // scaling factor of the view frustrum
    Real a = Real(img.width) / img.height;
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = background;
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    auto project = [&](Vector3 &p) -> Vector2 {
        return {-p.x / p.z, -p.y / p.z};
    };

    auto to_screen = [&](Vector2 &p) -> Vector2 {
        Real x_screen = img.width * (p.x + s * a) / (2.0 * s * a);
        Real y_screen = img.height * (s - p.y) / (2.0  * s);
        return {x_screen, y_screen};
    };
    for (int f = 0; f < mesh.faces.size(); f++) {
        Vector3 face = mesh.faces[f];

        Vector3 p0 = mesh.vertices[face[0]];
        Vector3 p1 = mesh.vertices[face[1]];
        Vector3 p2 = mesh.vertices[face[2]];

        Vector3 C0 = mesh.vertex_colors[face[0]];
        Vector3 C1 = mesh.vertex_colors[face[1]];
        Vector3 C2 = mesh.vertex_colors[face[2]];
        
        Vector2 p0_proj = project(p0);
        Vector2 p1_proj = project(p1);
        Vector2 p2_proj = project(p2);
    
        p0_proj = to_screen(p0_proj);
        p1_proj = to_screen(p1_proj);
        p2_proj = to_screen(p2_proj);

        // using floor and ceil to aligned with pixel grid
        int min_x = std::max(0, (int)std::floor(std::min({p0_proj.x, p1_proj.x, p2_proj.x})));
        int max_x = std::min(img.width - 1, (int)std::ceil(std::max({p0_proj.x, p1_proj.x, p2_proj.x})));
        int min_y = std::max(0, (int)std::floor(std::min({p0_proj.y, p1_proj.y, p2_proj.y})));
        int max_y = std::min(img.height - 1, (int)std::ceil(std::max({p0_proj.y, p1_proj.y, p2_proj.y})));

        Real area_proj = 0.5 * cross(p1_proj - p0_proj, p2_proj - p0_proj); //adding cross definition for (Vector2, Vector2)

        for (int y = min_y; y < max_y + 1; y++) {
            for (int x = min_x; x < max_x + 1; x++) {
                Vector3 antialiasing_color = {0.0, 0.0, 0.0};
                bool in_triangle = false;
                for (int i = 0; i < samples; i++) {
                    for (int j = 0; j < samples; j++) {
                        Real sample_x = x + (j + 0.5) * step;
                        Real sample_y = y + (i + 0.5) * step;
                        Vector2 p_proj = {sample_x, sample_y};
                        Real b0_proj = (0.5 * cross(p2_proj - p1_proj, p_proj - p1_proj)) / area_proj;
                        Real b1_proj = (0.5 * cross(p0_proj - p2_proj, p_proj - p2_proj)) / area_proj;
                        Real b2_proj = 1 - b1_proj - b0_proj;
                        if ( b0_proj >= 0 && b1_proj >= 0 && b2_proj >= 0) {
                            Real B = 1 / ( b0_proj / (p0.z) + b1_proj / (p1.z) + b2_proj / (p2.z) );
                            Real b0 = (b0_proj * B) / p0.z;
                            Real b1 = (b1_proj * B) / p1.z;
                            Real b2 = (b2_proj * B) / p2.z;
                            Real depth = b0 * p0.z + b1 * p1.z + b2 * p2.z;
                            Vector3 color = b0 * C0 + b1 * C1 + b2 * C2;
                            Real z_buffer_x = x * samples + j;
                            Real z_buffer_y = y * samples + i;
                            if (abs(depth) < z_buffer[z_buffer_x][z_buffer_y]) {
                                z_buffer[z_buffer_x][z_buffer_y] = abs(depth);
                                color_buffer[z_buffer_x][z_buffer_y] = color;
                            } 
                            in_triangle = true;
                        } 
                    }
                }
                if (in_triangle) {
                    for (int i = 0; i < samples; i++) {
                        for (int j = 0; j < samples; j++) {
                            Real z_buffer_x = x * samples + j;
                            Real z_buffer_y = y * samples + i;
                            antialiasing_color += color_buffer[z_buffer_x][z_buffer_y];
                        }
                    }
                    img(x, y) = antialiasing_color / Real(samples * samples);
                }
            }
            
        }

    }

    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }
    
    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);
    const int samples = 4;
    const Real step = Real(1) / samples;
    Vector3 background = scene.background;
    std::vector<std::vector<Real>> z_buffer((img.width * samples), std::vector<Real>((img.height * samples), std::numeric_limits<Real>::infinity()));
    std::vector<std::vector<Vector3>> color_buffer((img.width * samples), std::vector<Vector3>((img.height * samples), background));
    
    Real s = scene.camera.s; // scaling factor of the view frustrum
    Real a = Real(img.width) / img.height;
    Real z_near = scene.camera.z_near; // distance of the near clipping plane

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = background;
        }
    }
    
    auto point_vector = [&](Vector3 &p) -> Vector4 {
        return {p.x, p.y, p.z, 1.0};
    };

    auto to_point = [&](Vector4 &p) -> Vector3 {
        return {p.x, p.y, p.z};
    };

    auto project = [&](Vector3 &p) -> Vector2 {
        return {-p.x / p.z, -p.y / p.z};
    };

    auto to_screen = [&](Vector2 &p) -> Vector2 {
        Real x_screen = img.width * (p.x + s * a) / (2.0 * s * a);
        Real y_screen = img.height * (s - p.y) / (2.0  * s);
        return {x_screen, y_screen};
    };

    auto mesh = scene.meshes[0];
    for (int f = 0; f < scene.meshes[0].faces.size(); f++) {
        Vector3 face = mesh.faces[f];
        Matrix4x4 model_matrix = mesh.model_matrix;
        Matrix4x4 view_matrix = inverse(scene.camera.cam_to_world);

        Vector3 p0 = mesh.vertices[face[0]];
        Vector3 p1 = mesh.vertices[face[1]];
        Vector3 p2 = mesh.vertices[face[2]];
        std::cout << "f: " << f << std::endl;
        std::cout << "p0: " << p0 << std::endl;
        std::cout << "p1: " << p1 << std::endl;
        std::cout << "p2: " << p2 << std::endl;

        Vector4 p0_vector4 = point_vector(p0);
        Vector4 p1_vector4 = point_vector(p1);
        Vector4 p2_vector4 = point_vector(p2);

        Vector4 p0_ws = model_matrix * p0_vector4;
        Vector4 p1_ws = model_matrix * p1_vector4;
        Vector4 p2_ws = model_matrix * p2_vector4;

        Vector4 p0_camera_vec = view_matrix * p0_ws;
        Vector4 p1_camera_vec = view_matrix * p1_ws;
        Vector4 p2_camera_vec = view_matrix * p2_ws;

        Vector3 p0_camera = to_point(p0_camera_vec);
        Vector3 p1_camera = to_point(p1_camera_vec);
        Vector3 p2_camera = to_point(p2_camera_vec);

        Vector3 C0 = mesh.vertex_colors[face[0]];
        Vector3 C1 = mesh.vertex_colors[face[1]];
        Vector3 C2 = mesh.vertex_colors[face[2]];
        
        Vector2 p0_proj = project(p0_camera);
        Vector2 p1_proj = project(p1_camera);
        Vector2 p2_proj = project(p2_camera);
    
        p0_proj = to_screen(p0_proj);
        p1_proj = to_screen(p1_proj);
        p2_proj = to_screen(p2_proj);

        // using floor and ceil to aligned with pixel grid
        int min_x = std::max(0, (int)std::floor(std::min({p0_proj.x, p1_proj.x, p2_proj.x})));
        int max_x = std::min(img.width - 1, (int)std::ceil(std::max({p0_proj.x, p1_proj.x, p2_proj.x})));
        int min_y = std::max(0, (int)std::floor(std::min({p0_proj.y, p1_proj.y, p2_proj.y})));
        int max_y = std::min(img.height - 1, (int)std::ceil(std::max({p0_proj.y, p1_proj.y, p2_proj.y})));

        Real area_proj = 0.5 * cross(p1_proj - p0_proj, p2_proj - p0_proj); //adding cross definition for (Vector2, Vector2)
        
        for (int y = min_y; y < max_y + 1; y++) {
            for (int x = min_x; x < max_x + 1; x++) {
                Vector3 antialiasing_color = {0.0, 0.0, 0.0};
                bool in_triangle = false;
                for (int i = 0; i < samples; i++) {
                    for (int j = 0; j < samples; j++) {
                        Real sample_x = x + (j + 0.5) * step;
                        Real sample_y = y + (i + 0.5) * step;
                        Vector2 p_proj = {sample_x, sample_y};
                        Real b0_proj = (0.5 * cross(p2_proj - p1_proj, p_proj - p1_proj)) / area_proj;
                        Real b1_proj = (0.5 * cross(p0_proj - p2_proj, p_proj - p2_proj)) / area_proj;
                        Real b2_proj = 1 - b1_proj - b0_proj;
                        if ( b0_proj >= 0 && b1_proj >= 0 && b2_proj >= 0) {
                            Real B = 1 / ( b0_proj / (p0_camera.z) + b1_proj / (p1_camera.z) + b2_proj / (p2_camera.z) );
                            Real b0 = (b0_proj * B) / p0_camera.z;
                            Real b1 = (b1_proj * B) / p1_camera.z;
                            Real b2 = (b2_proj * B) / p2_camera.z;
                            Real depth = b0 * p0_camera.z + b1 * p1_camera.z + b2 * p2_camera.z;
                            Vector3 color = b0 * C0 + b1 * C1 + b2 * C2;
                            Real z_buffer_x = x * samples + j;
                            Real z_buffer_y = y * samples + i;
                            if (abs(depth) < z_buffer[z_buffer_x][z_buffer_y]) {
                                z_buffer[z_buffer_x][z_buffer_y] = abs(depth);
                                color_buffer[z_buffer_x][z_buffer_y] = color;
                            } 
                            in_triangle = true;
                        } 
                    }
                }
                if (in_triangle) {
                    for (int i = 0; i < samples; i++) {
                        for (int j = 0; j < samples; j++) {
                            Real z_buffer_x = x * samples + j;
                            Real z_buffer_y = y * samples + i;
                            antialiasing_color += color_buffer[z_buffer_x][z_buffer_y];
                        }
                    }
                    img(x, y) = antialiasing_color / Real(samples * samples);
                }
            }
            
        }
    }
    return img;
}

