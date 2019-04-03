def generate():
    import k40gen as m
    gens = m.Generators(21341, 1245, [7000., 700., 70., 0.])
    times = m.generate_k40(0, int(1e8), gens, "reference", False)
    assert(times.shape[0] == 4)


def test_generate():
    generate()
