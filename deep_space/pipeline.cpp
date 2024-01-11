#include "./pipeline.h"
#include "camera.h"

#include <iostream>

void init_camera(FramebufferContext &context, Node *node, Camera *camera) {
    context.set_framebuffer(camera->framebuffer());
    camera->start_frame(node->transform);
}
