all:

.PHONY: check-man hook
check-man:
	@for manpage in */*.[1-7]; do \
		echo "==> $$manpage <=="; \
		[ -z "$$(deroff -w $$manpage | ispell -l | tee /dev/stderr)" ] && echo "Success"; \
		echo ""; \
	done

hook:
	[ -e .git/hooks/pre-commit ] || ln pre-commit .git/hooks/pre-commit
