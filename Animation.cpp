#include "Animation.h"

Animation::Animation() 
    : first(0), last(0), cur(0), step(1), speed(0.1f), duration_left(0.1f), type(AnimationType::Loop) {}

Animation::Animation(int first_frame, int last_frame, int step_size, float anim_speed, AnimationType anim_type)
    : first(first_frame), last(last_frame), cur(first_frame), step(step_size), speed(anim_speed), duration_left(anim_speed), type(anim_type) {}

void Animation::update()
{
    float dt = GetFrameTime();
    this->duration_left -= dt;

    if (this->duration_left <= 0.0) {

        this->duration_left = this->speed;
        this->cur += this->step;

        if (this->cur > this->last)
        {
            switch (this->type)
            {
            case AnimationType::Loop:
                this->cur = this->first;
                break;
            case AnimationType::Once:
                this->cur = this->last;
                break;
            case AnimationType::PingPong:
                this->step = -this->step;
                this->cur += this->step * 2;
                break;
            }
        }
        else if (this->cur < this->first){

            switch (this->type)
            {
            case AnimationType::Loop:
                this->cur = this->last;
                break;
            case AnimationType::Once:
                this->cur = this->first;
                break;
            case AnimationType::PingPong:
                this->step = -this->step;
                this->cur += this->step * 2;
                break;
            }
        }
    }
}

Rectangle Animation::frame(int num_frames_per_row) {

    int x = (this->cur % num_frames_per_row) * 17;
    int y = (this->cur / num_frames_per_row) * 17;

    return (Rectangle){.x = (float)x, .y = (float)y, .width = 17.0, .height = 17.0};
}
