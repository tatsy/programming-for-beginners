/********************** WARNING *************************
 * This file includes INCOMPLETE implementation!
 * Be careful!!
 ********************************************************/

#pragma once

#include <vector>
#include <stack>
#include <tuple>

#include "common/vec3.h"

using Tuple = std::tuple<Vec3, double>;

struct BinaryTreeNode {
    bool isLeaf() {
        return left == nullptr && right == nullptr;
    }

    Vec3 point;
    double weight = 0.0;
    BinaryTreeNode *left = nullptr;
    BinaryTreeNode *right = nullptr;
};

class BinaryTree {
public:
    BinaryTree() {}
    BinaryTree(const std::vector<Vec3> &points, const std::vector<double> &weights) {
        construct(points, weights);
    }

    virtual ~BinaryTree() {
        for (auto node : nodes) {
            delete node;
        }
        nodes.clear();
    }

    double eval(const Vec3 &query, const std::function<double(const Vec3&, const Vec3&)> &f, double eps = 1.0e-6) const {
        std::stack<BinaryTreeNode*> node_stack;
        node_stack.push(root);

        double ret = 0.0;
        while (!node_stack.empty()) {
            auto node = node_stack.top();
            node_stack.pop();

            if (node->isLeaf()) {
                ret += node->weight * f(query, node->point);
            } else {
                bool ok = false;
                if (node->left && node->right) {
                    const double v1 = f(query, node->left->point);
                    const double v2 = f(query, node->right->point);
                    if ((v1 != 0.0 || v2 != 0.0) && std::abs(v1 - v2) < eps) {
                        ret += node->weight * f(query, node->point);
                        ok = true;
                    }
                }

                if (!ok) {
                    if (node->left) {
                        node_stack.push(node->left);
                    }

                    if (node->right) {
                        node_stack.push(node->right);
                    }
                }
            }
        }

        return ret;
    }

    void construct(const std::vector<Vec3> &points, const std::vector<double> &weights) {
        std::vector<Tuple> temp(points.size());
        for (size_t i = 0; i < points.size(); i++) {
            temp[i] = std::make_tuple(points[i], weights[i]);
        }
        root = constructRec(temp, 0, temp.size());
    }

private:
    BinaryTreeNode * constructRec(std::vector<Tuple> &points, int left, int right) {
        // Termination criteria
        if (left >= right) {
            return nullptr;
        }

        if (right - left == 1) {
            auto node = new BinaryTreeNode();
            node->point = std::get<0>(points[left]);
            node->weight = std::get<1>(points[left]);
            nodes.push_back(node);
            return node;
        }

        // maximum variance direction
        const int count = right - left;
        double muX = 0.0, muY = 0.0, muZ = 0.0;
        for (int i = left; i < right; i++) {
            const auto &p = std::get<0>(points[i]);
            muX += p.x / count;
            muY += p.y / count;
            muZ += p.z / count;
        }

        double varX = 0.0, varY = 0.0, varZ = 0.0;
        for (int i = left; i < right; i++) {
            const auto &p = std::get<0>(points[i]);
            varX += (p.x - muX) * (p.x - muX) / count;
            varY += (p.y - muY) * (p.y - muY) / count;
            varZ += (p.z - muZ) * (p.z - muZ) / count;
        }

        const double varMax = std::max(varX, std::max(varY, varZ));
        int maxAxis = 0;
        if (varMax == varY) maxAxis = 1;
        if (varMax == varZ) maxAxis = 2;

        // Sort along max axis
        std::sort(points.begin() + left, points.begin() + right, [&](const Tuple &v0, const Tuple &v1) {
            return std::get<0>(v0)[maxAxis] < std::get<0>(v1)[maxAxis];
        });

        // Traverse child nodes
        auto node = new BinaryTreeNode();
        node->point = Vec3(muX, muY, muZ);
        const int mid = (left + right) / 2;
        node->left = constructRec(points, left, mid);
        node->right = constructRec(points, mid, right);
        node->weight += node->left ? node->left->weight : 0.0;
        node->weight += node->right ? node->right->weight : 0.0;
        nodes.push_back(node);

        return node;
    }

    std::vector<BinaryTreeNode *> nodes;
    BinaryTreeNode *root = nullptr;
};

class FastMultipole {
public:
    FastMultipole() {}
    FastMultipole(const std::vector<Vec3> &points, const std::vector<double> &weights) {
        construct(points, weights);
    }

    void construct(const std::vector<Vec3> &points, const std::vector<double> &weights) {
        tree.construct(points, weights);
    }

    double eval(const Vec3 &query, const std::function<double(const Vec3&, const Vec3&)> &f, double eps = 1.0e-6) {
        return tree.eval(query, f, eps);
    }

private:
    BinaryTree tree;
};
