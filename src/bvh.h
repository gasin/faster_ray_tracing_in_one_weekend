#ifndef BVH_H
#define BVH_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>


class bvh_node : public hittable {
  public:
    bvh_node(const hittable_list& list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(const std::vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end) {
        auto objects = src_objects; // Create a modifiable array of the source scene objects

        auto comparator = box_x_compare;
        auto whole_bbox = objects[start]->bounding_box();
        for (auto ind = start+1; ind < end; ind++) {
            whole_bbox = aabb(whole_bbox, objects[ind]->bounding_box());
        }
        if (whole_bbox.x.size() > whole_bbox.y.size() && whole_bbox.x.size() > whole_bbox.z.size()) {
            comparator = box_x_compare;
        } else if (whole_bbox.y.size() > whole_bbox.z.size()) {
            comparator = box_y_compare;
        } else {
            comparator = box_z_compare;
        }

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            if (comparator(objects[start], objects[start+1])) {
                left = objects[start];
                right = objects[start+1];
            } else {
                left = objects[start+1];
                right = objects[start];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            double min_score = 1e60;
            auto min_mid = start+1;

            for (auto mid = start+1; mid < end; mid++) {
                auto bbox1 = objects[start]->bounding_box();
                for (auto ind = start+1; ind < mid; ind++) {
                    bbox1 = aabb(bbox1, objects[ind]->bounding_box());
                }
                auto bbox2 = objects[mid]->bounding_box();
                for (auto ind = mid+1; ind < end; ind++) {
                    bbox2 = aabb(bbox2, objects[ind]->bounding_box());
                }
                auto score = bbox1.surface_area() * (mid-start) + bbox2.surface_area() * (end-mid);

                if (score < min_score) {
                    min_score = score;
                    min_mid = mid;
                }
            }
            left = make_shared<bvh_node>(objects, start, min_mid);
            right = make_shared<bvh_node>(objects, min_mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override { return bbox; }

  private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    static bool box_compare(
        const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index
    ) {
        return a->bounding_box().axis(axis_index).min < b->bounding_box().axis(axis_index).min;
    }

    static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }
};


#endif
