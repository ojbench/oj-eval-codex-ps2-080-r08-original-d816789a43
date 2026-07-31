#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;
    int next;
    int from;
};

struct Frame {
    int u;
    int edge_it;
    int parent;
    int parent_edge;
    bool entered;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<int> head(n + 1, -1);
    vector<Edge> edges;
    edges.reserve(2LL * m);

    auto add_edge = [&](int u, int v) {
        int id = (int)edges.size();
        edges.push_back({v, head[u], u});
        head[u] = id;
    };

    for (int i = 0; i < m; ++i) {
        int x, y;
        cin >> x >> y;
        add_edge(x, y);
        add_edge(y, x);
    }

    vector<int> disc(n + 1, 0), low(n + 1, 0);
    vector<char> in_odd_cycle(n + 1, 0);
    vector<int> edge_stack;
    edge_stack.reserve(m);

    vector<int> seen_vertex(n + 1, 0), pos(n + 1, 0);
    int timer = 0;
    int comp_stamp = 0;

    auto process_component = [&](const vector<int>& comp_edges) {
        if (comp_edges.empty()) return;

        ++comp_stamp;
        vector<int> verts;
        verts.reserve(comp_edges.size() * 2);
        for (int e : comp_edges) {
            int a = edges[e].from;
            int b = edges[e].to;
            if (seen_vertex[a] != comp_stamp) {
                seen_vertex[a] = comp_stamp;
                verts.push_back(a);
            }
            if (seen_vertex[b] != comp_stamp) {
                seen_vertex[b] = comp_stamp;
                verts.push_back(b);
            }
        }

        for (int i = 0; i < (int)verts.size(); ++i) pos[verts[i]] = i;
        vector<vector<int>> adj(verts.size());
        adj.reserve(verts.size());
        for (int e : comp_edges) {
            int a = pos[edges[e].from];
            int b = pos[edges[e].to];
            adj[a].push_back(b);
            adj[b].push_back(a);
        }

        vector<int> color(verts.size(), -1);
        queue<int> q;
        bool bipartite = true;
        for (int i = 0; i < (int)verts.size() && bipartite; ++i) {
            if (color[i] != -1) continue;
            color[i] = 0;
            q.push(i);
            while (!q.empty() && bipartite) {
                int u = q.front();
                q.pop();
                for (int v : adj[u]) {
                    if (color[v] == -1) {
                        color[v] = color[u] ^ 1;
                        q.push(v);
                    } else if (color[v] == color[u]) {
                        bipartite = false;
                        break;
                    }
                }
            }
        }

        if (!bipartite) {
            for (int v : verts) in_odd_cycle[v] = 1;
        }
    };

    vector<Frame> st;
    st.reserve(n);

    for (int s = 1; s <= n; ++s) {
        if (disc[s] != 0) continue;
        st.push_back({s, head[s], -1, -1, false});

        while (!st.empty()) {
            Frame &f = st.back();
            if (!f.entered) {
                f.entered = true;
                disc[f.u] = low[f.u] = ++timer;
            }

            if (f.edge_it != -1) {
                int e = f.edge_it;
                f.edge_it = edges[e].next;

                if (e == (f.parent_edge ^ 1)) continue;

                int v = edges[e].to;
                if (disc[v] == 0) {
                    edge_stack.push_back(e);
                    st.push_back({v, head[v], f.u, e, false});
                } else if (disc[v] < disc[f.u]) {
                    edge_stack.push_back(e);
                    low[f.u] = min(low[f.u], disc[v]);
                }
            } else {
                Frame cur = f;
                st.pop_back();

                if (cur.parent != -1) {
                    low[cur.parent] = min(low[cur.parent], low[cur.u]);
                    if (low[cur.u] >= disc[cur.parent]) {
                        vector<int> comp_edges;
                        while (!edge_stack.empty()) {
                            int e = edge_stack.back();
                            edge_stack.pop_back();
                            comp_edges.push_back(e);
                            if (e == cur.parent_edge) break;
                        }
                        process_component(comp_edges);
                    }
                }
            }
        }
    }

    int answer = 0;
    for (int i = 1; i <= n; ++i) {
        if (!in_odd_cycle[i]) ++answer;
    }
    cout << answer << '\n';
    return 0;
}
