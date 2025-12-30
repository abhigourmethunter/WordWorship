#pragma once
#include <raylib.h>

class Animation {
public:
    enum class AnimationType {
        Loop,
        Once,
        PingPong
    };

    int first;
    int last;
    int cur;
    int step;
    float speed;
    float duration_left; 
    AnimationType type;

    Animation();
    Animation(int first_frame, int last_frame, int step_size, float anim_speed, AnimationType anim_type);
    void update();
    Rectangle frame(int num_frames_per_row);
};