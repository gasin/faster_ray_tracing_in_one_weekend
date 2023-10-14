#ifndef QBVH_H
#define QBVH_H

#include "rtweekend.h"

#include "hittable.h"
#include "bvh.h"

#include "immintrin.h"

int counter = 0;

class qbvh_node : public hittable {
  public:

    qbvh_node(shared_ptr<bvh_node> bvh_root) {
        separator[0] = bvh_root->sep;

        aabb bbox_[4];
        qbvh_node_half(bvh_root->right, typeid(*bvh_root).name(), 0, bbox_);
        qbvh_node_half(bvh_root->left, typeid(*bvh_root).name(), 2, bbox_);

        bbox[0][0] = _mm256_set_pd(bbox_[3].x.min, bbox_[2].x.min, bbox_[1].x.min, bbox_[0].x.min);
        bbox[0][1] = _mm256_set_pd(bbox_[3].x.max, bbox_[2].x.max, bbox_[1].x.max, bbox_[0].x.max);
        bbox[1][0] = _mm256_set_pd(bbox_[3].y.min, bbox_[2].y.min, bbox_[1].y.min, bbox_[0].y.min);
        bbox[1][1] = _mm256_set_pd(bbox_[3].y.max, bbox_[2].y.max, bbox_[1].y.max, bbox_[0].y.max);
        bbox[2][0] = _mm256_set_pd(bbox_[3].z.min, bbox_[2].z.min, bbox_[1].z.min, bbox_[0].z.min);
        bbox[2][1] = _mm256_set_pd(bbox_[3].z.max, bbox_[2].z.max, bbox_[1].z.max, bbox_[0].z.max);
    }

    void qbvh_node_half(shared_ptr<hittable> hittable_root, const char *bvh_name, int index, aabb *bbox_) {
        if (typeid(*hittable_root).name() != bvh_name) {
            bbox_[index] = hittable_root->bounding_box();
            bbox_[index+1] = hittable_root->bounding_box();
            child[index] = hittable_root;
            child[index+1] = hittable_root;
            return;
        }
        shared_ptr<bvh_node> bvh_root = std::dynamic_pointer_cast<bvh_node>(hittable_root);
        qbvh_node_quarter(bvh_root->right, bvh_name, index, bbox_);
        qbvh_node_quarter(bvh_root->left, bvh_name, index+1, bbox_);
    }

    void qbvh_node_quarter(shared_ptr<hittable> hittable_root, const char *bvh_name, int index, aabb *bbox_) {
        if (typeid(*hittable_root).name() != bvh_name) {
            bbox_[index] = hittable_root->bounding_box();
            child[index] = hittable_root;
            return;
        }
        shared_ptr<bvh_node> bvh_root = std::dynamic_pointer_cast<bvh_node>(hittable_root);
        bbox_[index] = bvh_root->bounding_box();
        child[index] = make_shared<qbvh_node>(bvh_root);
    }

    __m256d intersect(const ray& r, interval ray_t) const {
        __m256d t0 = _mm256_set1_pd(ray_t.min), t1 = _mm256_set1_pd(ray_t.max);
        __m256d invD[3] = {
            _mm256_set1_pd(r.inv_direction()[0]),
            _mm256_set1_pd(r.inv_direction()[1]),
            _mm256_set1_pd(r.inv_direction()[2]),
        };
        __m256d orig[3] = {
            _mm256_set1_pd(r.origin()[0]),
            _mm256_set1_pd(r.origin()[1]),
            _mm256_set1_pd(r.origin()[2]),
        };

        for (int a = 0; a < 3; a++) {
            __m256d ta = _mm256_mul_pd(_mm256_sub_pd(bbox[a][0], orig[a]), invD[a]);
            __m256d tb = _mm256_mul_pd(_mm256_sub_pd(bbox[a][1], orig[a]), invD[a]);
            t0 = _mm256_max_pd(_mm256_min_pd(ta, tb), t0);
            t1 = _mm256_min_pd(_mm256_max_pd(ta, tb), t1);
        }
        return _mm256_cmp_pd(t0, t1, 2);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        __m256d is_box_hit = intersect(r, ray_t);
        union myUnion {
            double d;
            uint64_t x;
        };
        myUnion myunions[4] = {is_box_hit[0], is_box_hit[1], is_box_hit[2], is_box_hit[3] };
        bool is_hit = false;
        for (int i = 0; i < 4; i++) {
            if (myunions[i].x == 0) {
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
    __m256d bbox[3][2];
    shared_ptr<hittable> child[4];
    int separator[3];
};


#endif
