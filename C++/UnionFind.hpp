class UnionFind {
    int *_id;
    int *_size;

public:
    explicit UnionFind(int n) {
        _id = new int[n];
        _size = new int[n];
        for (int i = 0; i < n; i++) {
            _id[i] = i;
            _size[i] = 1;
        }
    }

    ~UnionFind() {
        delete[] _id;
        delete[] _size;
    }

    int find(int x) {

        int root = x;
        // Find root
        while (root != _id[root]) root = _id[root];

        // Compress path to root
        while (x != root) {
            int temp = _id[x];
            _id[x] = root;
            x = temp;
        }
        return root;
    }

    void merge(int x, int y) {
        int i = find(x);
        int j = find(y);
        if (i == j) return;

        // Make smaller root point to larger one
        if (_size[i] < _size[j]) {
            _id[i] = j;
            _size[j] += _size[i];
        } else {
            _id[j] = i;
            _size[i] += _size[j];
        }
    }

    bool areConnected(int x, int y) {
        return find(x) == find(y);
    }
};