def perebor_matr():
    len_min = -1
    c = []
    res = []
    n = len(distances)

    for i in range(n):
        c.append(i)
        res.append(0)

    for k in range(n):
        len1 = 0
        for i in range(n - 1):
            len1 += distances[c[i]][c[i + 1]]

        len1 += distances[c[n - 1]][c[0]]
        if len1 < len_min or len_min < 0:
            len_min = len1
            for i in range(n):
                res[i] = c[i]
    return res
