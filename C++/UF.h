class UF {
    int *id;
    int *size;

public:
    UF(int n) {
        id = new int[n];
        size = new int[n];
        for (int i = 0; i < n; i++) {
            id[i] = i;
            size[i] = 1;
        }
    }

    ~UF() {
        delete[] id;
        delete[] size;
    }

    int find(int x) {

        int root = x;
        // Find root
        while (root != id[root])
            root = id[root];

        // Compress path to root
        while (x != root) {
            int temp = id[x];
            id[x] = root;
            x = temp;
        }
        return root;
    }

    void merge(int x, int y) {
        int i = find(x);
        int j = find(y);
        if (i == j) return;

        // Make smaller root point to larger one
        if (size[i] < size[j]) {
            id[i] = j;
            size[j] += size[i];
        } else {
            id[j] = i;
            size[i] += size[j];
        }
    }

    bool areConnected(int x, int y) {
        return find(x) == find(y);
    }
};