#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Vector3 background = Vector3{0.5, 0.5, 0.5};
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    for (int y = 0; y < img.height; y++) {
        Real dy = y + Real(0.5) - center.y;
        for (int x = 0; x < img.width; x++) {
            Real dx = x + Real(0.5) - center.x;
            if (dy * dy + dx * dx <= radius * radius) {
                img(x, y) = color;
            } else {
                img(x, y) = background;
            }
        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 background = scene.background;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = background;
        }
    }
    
    for (const auto &circle : scene.objects) {
        int min_x = std::max(0, int(circle.center.x - circle.radius));
        int max_x = std::min(img.width - 1, int(circle.center.x + circle.radius));
        int min_y = std::max(0, int(circle.center.y - circle.radius));
        int max_y = std::min(img.height - 1, int(circle.center.y + circle.radius));
        Vector2 center = circle.center;
        Real radius = circle.radius;
        Vector3 color = circle.color;
        for (int y = min_y; y < max_y; y++) {
            Real dy = y + Real(0.5) - center.y;
            for (int x = min_x; x < max_x; x++) {
                Real dx = x + Real(0.5) - center.x;
                if (dy * dy + dx * dx <= radius * radius) {
                    img(x, y) = color;
                } 
            }
        }
    }
    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 background = scene.background;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = background;
        }
    }

    for (const auto &shape : scene.shapes) {
        if (auto *circle = std::get_if<Circle>(&shape)) {
            Vector2 center = circle->center;
            Real radius = circle->radius;
            Vector3 color = circle->color;
            for (int y = 0; y < img.height; y++) {
                Real dy = y + Real(0.5) - center.y;
                for (int x = 0; x < img.width; x++) {
                    Real dx = x + Real(0.5) - center.x;
                    if (dy * dy + dx * dx <= radius * radius) {
                        img(x, y) = color;
                    } 
                }
            }
        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
            Vector2 p_min = rectangle->p_min;
            Vector2 p_max = rectangle->p_max;
            Vector3 color = rectangle->color;
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    if (x + Real(0.5) >= p_min.x && x + Real(0.5) <= p_max.x && y + Real(0.5) >= p_min.y && y+ Real(0.5) <= p_max.y) {
                        img(x,y) = color;
                    }
                }
            }
        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
            Vector2 p0 = triangle->p0;
            Vector2 p1 = triangle->p1;
            Vector2 p2 = triangle->p2;
            Vector3 color = triangle->color;

            Vector2 v01 = p1 - p0;
            Vector2 v12 = p2 - p1;
            Vector2 v20 = p0 - p2;

            Vector2 n01 = {v01.y, -v01.x};
            Vector2 n12 = {v12.y, -v12.x};
            Vector2 n20 = {v20.y, -v20.x};
 
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector2 q = {x+ Real(0.5), y+ Real(0.5)};
                    Real d01 = dot((q - p0), n01);
                    Real d12 = dot((q - p1), n12);
                    Real d20 = dot((q - p2), n20);
                    if ((d01 >= 0 && d12 >= 0 && d20 >= 0) || (d01 <= 0 && d12 <= 0 && d20 <= 0)){
                        img(x, y) = color;
                    }
                }
            }
        }
    }
    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 background = scene.background;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = background;
        }
    }

    for (const auto &shape : scene.shapes) {
        if (auto *circle = std::get_if<Circle>(&shape)) {
            Vector2 center = circle->center;
            Real radius = circle->radius;
            Vector3 color = circle->color;
            Matrix3x3 transform = circle->transform;
            Matrix3x3 inv_transform = inverse(transform);
           
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 screen_vector = {
                        (x + 0.5), (y + 0.5), Real(1)
                    };
                    Vector3 object_vector = inv_transform * screen_vector;

                    Real dy = object_vector.x - center.y;
                    Real dx = object_vector.y - center.x;
                    if (dy * dy + dx * dx <= radius * radius) {
                        img(x, y) = color;
                    } 
                }
            }
        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
            Vector2 p_min = rectangle->p_min;
            Vector2 p_max = rectangle->p_max;
            Vector3 color = rectangle->color;
            Matrix3x3 transform = rectangle->transform;
            Matrix3x3 inv_transform = inverse(transform);

            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 screen_vector = {
                        (x + 0.5), (y + 0.5), Real(1)
                    };
                    Vector3 object_vector = inv_transform * screen_vector;
                    if (object_vector.x >= p_min.x && object_vector.x <= p_max.x && object_vector.y >= p_min.y && object_vector.y <= p_max.y) {
                        img(x,y) = color;
                    }
                }
            }
        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
            Vector2 p0 = triangle->p0;
            Vector2 p1 = triangle->p1;
            Vector2 p2 = triangle->p2;
            Vector3 color = triangle->color;

            Vector2 v01 = p1 - p0;
            Vector2 v12 = p2 - p1;
            Vector2 v20 = p0 - p2;

            Vector2 n01 = {v01.y, -v01.x};
            Vector2 n12 = {v12.y, -v12.x};
            Vector2 n20 = {v20.y, -v20.x};

            Matrix3x3 transform = triangle->transform;
            Matrix3x3 inv_transform = inverse(transform);
 
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 screen_vector = {
                        (x + 0.5), (y + 0.5), Real(1)
                    };
                    Vector3 object_vector = inv_transform * screen_vector;
                    Vector2 q = {object_vector.x, object_vector.y};
                    Real d01 = dot((q - p0), n01);
                    Real d12 = dot((q - p1), n12);
                    Real d20 = dot((q - p2), n20);
                    if ((d01 >= 0 && d12 >= 0 && d20 >= 0) || (d01 <= 0 && d12 <= 0 && d20 <= 0)){
                        img(x, y) = color;
                    }
                }
            }
        }
    }
    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    const int samples = 4;
    const Real step = Real(1) / samples;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 antialiasing_color = {0.0, 0.0, 0.0};
            for (int i = 0; i < samples; i++) {
                for (int j = 0; j < samples; j++) {
                    Real subpixel_x = Real(x) + (j + 0.5) * step;
                    Real subpixel_y = Real(y) + (i + 0.5) * step;
                    Vector3 color = scene.background;
                    for (const auto &shape : scene.shapes) {
                        if (auto *circle = std::get_if<Circle>(&shape)) {
                            Vector2 center = circle->center;
                            Real radius = circle->radius;
                            Matrix3x3 transform = circle->transform;
                            Matrix3x3 inv_transform = inverse(transform);
                            Vector3 screen_vector = {
                                subpixel_x, subpixel_y, Real(1)
                            };
                            Vector3 object_vector = inv_transform * screen_vector;
                            Real dy = object_vector.x - center.y;
                            Real dx = object_vector.y - center.x;
                            if (dy * dy + dx * dx <= radius * radius) {
                                color = circle->color;
                            } 


                        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                            Vector2 p_min = rectangle->p_min;
                            Vector2 p_max = rectangle->p_max;
                            Matrix3x3 transform = rectangle->transform;
                            Matrix3x3 inv_transform = inverse(transform);
                            Vector3 screen_vector = {
                                subpixel_x, subpixel_y, Real(1)
                            };
                            Vector3 object_vector = inv_transform * screen_vector;
                            if (object_vector.x >= p_min.x && object_vector.x <= p_max.x && object_vector.y >= p_min.y && object_vector.y <= p_max.y) {
                                color = rectangle->color;
                            }

                        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                            Vector2 p0 = triangle->p0;
                            Vector2 p1 = triangle->p1;
                            Vector2 p2 = triangle->p2;

                            Vector2 v01 = p1 - p0;
                            Vector2 v12 = p2 - p1;
                            Vector2 v20 = p0 - p2;

                            Vector2 n01 = {v01.y, -v01.x};
                            Vector2 n12 = {v12.y, -v12.x};
                            Vector2 n20 = {v20.y, -v20.x};

                            Matrix3x3 transform = triangle->transform;
                            Matrix3x3 inv_transform = inverse(transform);
                            Vector3 screen_vector = {
                                subpixel_x, subpixel_y, Real(1)
                            };
                            Vector3 object_vector = inv_transform * screen_vector;
                            Vector2 q = {object_vector.x, object_vector.y};
                            Real d01 = dot((q - p0), n01);
                            Real d12 = dot((q - p1), n12);
                            Real d20 = dot((q - p2), n20);
                            if ((d01 >= 0 && d12 >= 0 && d20 >= 0) || (d01 <= 0 && d12 <= 0 && d20 <= 0)){
                                color = triangle->color;
                            }
                        }
                    }
                    antialiasing_color += color;
                }
            }
            img(x, y) = antialiasing_color / Real(samples * samples);
        }
    }
    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    const int samples = 4;
    const Real step = Real(1) / samples;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 antialiasing_color = {0.0, 0.0, 0.0};
            for (int i = 0; i < samples; i++) {
                for (int j = 0; j < samples; j++) {
                    Real subpixel_x = Real(x) + (j + 0.5) * step;
                    Real subpixel_y = Real(y) + (i + 0.5) * step;
                    Vector3 color = scene.background;
                    for (const auto &shape : scene.shapes) {
                        if (auto *circle = std::get_if<Circle>(&shape)) {
                            Vector2 center = circle->center;
                            Real radius = circle->radius;
                            Matrix3x3 transform = circle->transform;
                            Matrix3x3 inv_transform = inverse(transform);
                            Vector3 screen_vector = {
                                subpixel_x, subpixel_y, Real(1)
                            };
                            Vector3 object_vector = inv_transform * screen_vector;
                            Real dy = object_vector.x - center.y;
                            Real dx = object_vector.y - center.x;
                            if (dy * dy + dx * dx <= radius * radius) {
                                color = circle->color * circle->alpha + (1 - circle->alpha) * color;
                            } 


                        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                            Vector2 p_min = rectangle->p_min;
                            Vector2 p_max = rectangle->p_max;
                            Matrix3x3 transform = rectangle->transform;
                            Matrix3x3 inv_transform = inverse(transform);
                            Vector3 screen_vector = {
                                subpixel_x, subpixel_y, Real(1)
                            };
                            Vector3 object_vector = inv_transform * screen_vector;
                            if (object_vector.x >= p_min.x && object_vector.x <= p_max.x && object_vector.y >= p_min.y && object_vector.y <= p_max.y) {
                                color = rectangle->color * rectangle->alpha + (1 - rectangle->alpha) * color;
                            }

                        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                            Vector2 p0 = triangle->p0;
                            Vector2 p1 = triangle->p1;
                            Vector2 p2 = triangle->p2;

                            Vector2 v01 = p1 - p0;
                            Vector2 v12 = p2 - p1;
                            Vector2 v20 = p0 - p2;

                            Vector2 n01 = {v01.y, -v01.x};
                            Vector2 n12 = {v12.y, -v12.x};
                            Vector2 n20 = {v20.y, -v20.x};

                            Matrix3x3 transform = triangle->transform;
                            Matrix3x3 inv_transform = inverse(transform);
                            Vector3 screen_vector = {
                                subpixel_x, subpixel_y, Real(1)
                            };
                            Vector3 object_vector = inv_transform * screen_vector;
                            Vector2 q = {object_vector.x, object_vector.y};
                            Real d01 = dot((q - p0), n01);
                            Real d12 = dot((q - p1), n12);
                            Real d20 = dot((q - p2), n20);
                            if ((d01 >= 0 && d12 >= 0 && d20 >= 0) || (d01 <= 0 && d12 <= 0 && d20 <= 0)){
                                color = triangle->color * triangle->alpha + (1 - triangle->alpha) * color;
                            }
                        }
                    }
                    antialiasing_color += color;
                }
            }
            img(x, y) = antialiasing_color / Real(samples * samples);
        }
    }
    return img;
}
