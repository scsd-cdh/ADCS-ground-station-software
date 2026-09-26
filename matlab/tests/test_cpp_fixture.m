% test_cpp_fixture  The committed C++ reference (cpp/tests/data/demo_reference.txt)
% must equal what MATLAB produces now. Catches a stale or hand-edited fixture.
% Slower than scripts/check_cpp_fixture.py (runs the demo scenario) but exact.
committed = fullfile(fileparts(fileparts(fileparts(mfilename('fullpath')))), 'cpp', 'tests', 'data', 'demo_reference.txt');
fresh = [tempname '.txt'];
export_cpp_fixture(fresh);
a = readlines(committed);
b = readlines(fresh);
delete(fresh);
assert(numel(a) == numel(b), 'fixture line count differs - run export_cpp_fixture');
for k = 1:numel(a)
    ta = split(strtrim(a(k)));  tb = split(strtrim(b(k)));
    assert(numel(ta) == numel(tb) && ta(1) == tb(1), 'fixture line %d differs - run export_cpp_fixture', k);
    na = str2double(ta(2:end));  nb = str2double(tb(2:end));
    if all(isfinite(na)) && ~isempty(na)
        assert(max(abs(na - nb) ./ max(1, abs(na))) < 1e-9, 'fixture line %d differs - run export_cpp_fixture', k);
    else
        assert(isequal(ta, tb), 'fixture line %d differs - run export_cpp_fixture', k);
    end
end
disp('test_cpp_fixture: fixture is current');
