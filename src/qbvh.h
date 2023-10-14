#ifndef QBVH_H
#define QBVH_H

#include "rtweekend.h"

#include "hittable.h"
#include "bvh.h"

int counter = 0;

class qbvh_node : public hittable {
  public:

    qbvh_node(shared_ptr<bvh_node> bvh_root) {
        separator[0] = bvh_root->sep;

        qbvh_node_half(bvh_root->right, typeid(*bvh_root).name(), 0);
        qbvh_node_half(bvh_root->left, typeid(*bvh_root).name(), 2);
    }

    void qbvh_node_half(shared_ptr<hittable> hittable_root, const char *bvh_name, int index) {
        if (typeid(*hittable_root).name() != bvh_name) {
            bbox[index] = hittable_root->bounding_box();
            bbox[index+1] = hittable_root->bounding_box();
            child[index] = hittable_root;
            child[index+1] = hittable_root;
            return;
        }
        shared_ptr<bvh_node> bvh_root = std::dynamic_pointer_cast<bvh_node>(hittable_root);
        qbvh_node_quarter(bvh_root->right, bvh_name, index);
        qbvh_node_quarter(bvh_root->left, bvh_name, index+1);
    }

    void qbvh_node_quarter(shared_ptr<hittable> hittable_root, const char *bvh_name, int index) {
        if (typeid(*hittable_root).name() != bvh_name) {
            bbox[index] = hittable_root->bounding_box();
            child[index] = hittable_root;
            return;
        }
        shared_ptr<bvh_node> bvh_root = std::dynamic_pointer_cast<bvh_node>(hittable_root);
        bbox[index] = bvh_root->bounding_box();
        child[index] = make_shared<qbvh_node>(bvh_root);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        bool is_hit = false;
        for (int i = 0; i < 4; i++) {
            if (!bbox[i].hit(r, ray_t)) {
                continue;
            }
            if (child[i]->hit(r, ray_t, rec)) {
                is_hit = true;
                ray_t.max = rec.t;
            }
        }
        return is_hit;
    }

    aabb bounding_box() const override { return aabb(); }

  private:
    aabb bbox[4];
    shared_ptr<hittable> child[4];
    int separator[3];
};


#endif
