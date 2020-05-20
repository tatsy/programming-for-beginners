#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <functional>

enum KnnSearchType {
    EPSILON_BALL = 0x01,
    K_NEAREST    = 0x02
};

struct KnnQuery {
    KnnQuery(int type, double epsilon, int k)
            : type(static_cast<KnnSearchType>(type))
            , epsilon(epsilon)
            , k(k) {
    }

    KnnSearchType type;
    double epsilon;
    int k;
};

template <typename T>
struct KDTreeNode {
    bool isLeaf() const {
        return axis == -1;
    }

    T point;
    KDTreeNode<T> *left = nullptr;
    KDTreeNode<T> *right = nullptr;
    int axis = -1;
};

template <typename T>
class KDTree {
public:
    KDTree() {}
    virtual ~KDTree() {
        for (KDTreeNode<T> *node : nodes) {
            delete node;
        }
        nodes.clear();
    }

    void construct(const std::vector<T>& points) {
        std::vector<T> copy(points.begin(), points.end());
        root = constructRec(copy, 0, copy.size());
    }

    T nearest(const T& point) const {
        std::stack<KDTreeNode<T>*> nodes;
        nodes.push(root);

        T ret;
        double minDist = 1.0e20;
        while (!nodes.empty()) {
            KDTreeNode<T> *node = nodes.top();
            nodes.pop();

            const double dist = length(point - node->point);
            if (dist < minDist) {
                ret = node->point;
                minDist = dist;
            }

            if (!node->isLeaf()) {
                const double p = point[node->axis];
                const double q = node->point[node->axis];
                const double len = std::abs(p - q);
                if (p < q || len < minDist) {
                    if (node->left) {
                        nodes.push(node->left);
                    }
                }

                if (p >= q || len < minDist) {
                    if (node->right) {
                        nodes.push(node->right);
                    }
                }
            }
        }

        return ret;
    }

private:
    KDTreeNode<T> *constructRec(std::vector<T> &points, int left, int right) {
        // Termination criteria
        if (left >= right) {
            return nullptr;
        }

        if (right - left == 1) {
            auto node = new KDTreeNode<T>();
            node->point = points[left];
        }

        // maximum variance direction
        const int count = right - left;
        double muX = 0.0, muY = 0.0, muZ = 0.0;
        for (int i = left; i < right; i++) {
            muX += points[i].x / count;
            muY += points[i].y / count;
            muZ += points[i].z / count;
        }

        double varX = 0.0, varY = 0.0, varZ = 0.0;
        for (int i = left; i < right; i++) {
            varX += (points[i].x - muX) * (points[i].x - muX) / count;
            varY += (points[i].y - muY) * (points[i].y - muY) / count;
            varZ += (points[i].z - muZ) * (points[i].z - muZ) / count;
        }

        const double varMax = std::max(varX, std::max(varY, varZ));
        int maxAxis = 0;
        if (varMax == varY) maxAxis = 1;
        if (varMax == varZ) maxAxis = 2;

        // Sort
        sort(points.begin() + left, points.begin() + right, [&](const T& p, const T& q){
            return p[maxAxis] < q[maxAxis];
        });

        // Node
        const int mid = (left + right) / 2;
        auto node = new KDTreeNode<T>();
        node->axis = maxAxis;
        node->point = points[(left + right) / 2];
        node->left = constructRec(points, left, mid);
        node->right = constructRec(points, mid + 1, right);
        nodes.push_back(node);

        return node;
    }

    KDTreeNode<T> *root = nullptr;
    std::vector<KDTreeNode<T>*> nodes;
};
