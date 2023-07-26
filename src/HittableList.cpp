#include "HittableList.hpp"

bool HittableList::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    HitRecord tempRec;
    bool hitAnything = false;
    auto closest = tMax;

    for (const auto& object : objects) {
        if (object->hit(r, tMin, closest, tempRec)) {
            // TODO: also need to pass object's colour back
            hitAnything = true;
            closest = tempRec.t;
            rec = tempRec;
            rec.color = object->Albedo;
        }
    }

    return hitAnything;
}
