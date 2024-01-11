#ifndef NOISE
#define NOISE

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/mat2x2.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <span>

void sample3d(std::vector<unsigned char> &data, int width, int height, int depth,
              std::function<float(glm::vec3)> noise);

template <unsigned int N> struct Dim {};

template <> struct Dim<2> {
    typedef glm::mat2 Mat;
};

template <> struct Dim<3> {
    typedef glm::mat3 Mat;
};

template <unsigned int N> std::pair<typename Dim<N>::Mat, typename Dim<N>::Mat> simplex_matrix() {
    const float n = N;
    const float skew_factor = (sqrt(n + 1.0) - 1.0) / n;
    const float unskew_factor = (1.0 - (1.0 / sqrt(n + 1.0))) / n;

    typename Dim<N>::Mat skew(1.0);
    typename Dim<N>::Mat unskew(1.0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            skew[i][j] += skew_factor;
            unskew[i][j] -= unskew_factor;
        }
    }

    return std::pair(skew, unskew);
}

template <typename T>
void permutator_impl(std::vector<T> &span,
                     const std::function<void(const std::vector<T> &)> &terminator,
                     unsigned int depth) {
    if (depth == 0) {
        terminator(span);
        return;
    }

    permutator_impl(span, terminator, depth - 1);

    for (int i = 0; i < depth; i++) {
        if (depth % 2) {
            std::swap(span[depth], span[i]);
        } else {
            std::swap(span[depth], span[0]);
        }
        permutator_impl(span, terminator, depth - 1);
    }
}

template <typename T>
void permute(std::vector<T> &span, const std::function<void(const std::vector<T> &)> &terminator) {
    permutator_impl(span, terminator, span.size() - 1);
}

#endif /*NOISE*/
