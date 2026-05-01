#pragma once

#include "Common.h"

#include <glm/glm.hpp>

#include "Scene/Scene.h"

struct LerpWaterObject
{
    u32 objIdx;
    glm::vec3 startPos;
    glm::vec3 endPos;
    f32 startTime;
    f32 duration;
    f32 expectedEndTime;
    bool playing = false;

    void start(f32 duration_, f32 startTime_, u32 objIdx_, glm::vec3 startPos_, glm::vec3 endPos_)
    {
        duration = duration_;
        startTime = startTime_;
        objIdx = objIdx_;
        startPos = startPos_;
        endPos = endPos_;

        expectedEndTime = startTime + duration;

        playing = true;
    }


    // note(ilja): returns true if ended, false if it's in process
    bool update(f32 currentTime, Scene& scene)
    {
        if (!playing)
            return true;
        
        if (currentTime >= expectedEndTime)
        {
            scene.move_object(objIdx, endPos);
            playing = false;
            
            return true;
        }

        f32 elapsed = currentTime - startTime;
        f32 factor = elapsed / duration;

        glm::vec3 pos = (1 - factor) * startPos + factor * endPos;

        scene.move_object(objIdx, pos);

        return false;
    }
};