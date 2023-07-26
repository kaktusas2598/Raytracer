#pragma once

#include "Hittable.hpp"

#include <memory>
#include <vector>

using std::shared_ptr;

class HittableList : public Hittable {
    public:
        HittableList() {}
        HittableList(shared_ptr<Hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<Hittable> object) { objects.push_back(object); }

        virtual bool hit(
            const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

        const std::vector<shared_ptr<Hittable>>& getObjects() const { return objects; }
    public:
        std::vector<shared_ptr<Hittable>> objects;
};
