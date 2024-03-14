--
-- PostgreSQL database dump
--

-- Dumped from database version 16.1
-- Dumped by pg_dump version 16.1

-- Started on 2024-03-15 00:44:11

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- TOC entry 216 (class 1259 OID 16439)
-- Name: chats; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.chats (
    user_1_id integer,
    user_2_id integer,
    chat_vector bytea,
    date timestamp with time zone
);


ALTER TABLE public.chats OWNER TO postgres;

--
-- TOC entry 220 (class 1259 OID 16512)
-- Name: chats_info; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.chats_info (
    user_id bigint,
    chat_names_vector bytea,
    date timestamp with time zone
);


ALTER TABLE public.chats_info OWNER TO postgres;

--
-- TOC entry 219 (class 1259 OID 16497)
-- Name: file_names; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.file_names (
    user_1_id bigint,
    user_2_id bigint,
    file_names_vector bytea,
    date timestamp with time zone
);


ALTER TABLE public.file_names OWNER TO postgres;

--
-- TOC entry 221 (class 1259 OID 16522)
-- Name: files; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.files (
    user_1_id bigint,
    user_2_id bigint,
    file_name character varying(255),
    file_data bytea,
    date timestamp with time zone
);


ALTER TABLE public.files OWNER TO postgres;

--
-- TOC entry 217 (class 1259 OID 16454)
-- Name: useraccdata; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.useraccdata (
    user_id integer,
    acc_data_vector bytea,
    date timestamp with time zone
);


ALTER TABLE public.useraccdata OWNER TO postgres;

--
-- TOC entry 215 (class 1259 OID 16429)
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.users (
    nickname character varying(255) NOT NULL,
    password character varying(255) NOT NULL,
    id integer NOT NULL
);


ALTER TABLE public.users OWNER TO postgres;

--
-- TOC entry 218 (class 1259 OID 16466)
-- Name: users_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.users_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.users_id_seq OWNER TO postgres;

--
-- TOC entry 4822 (class 0 OID 0)
-- Dependencies: 218
-- Name: users_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.users_id_seq OWNED BY public.users.id;


--
-- TOC entry 4654 (class 2604 OID 16467)
-- Name: users id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users ALTER COLUMN id SET DEFAULT nextval('public.users_id_seq'::regclass);


--
-- TOC entry 4811 (class 0 OID 16439)
-- Dependencies: 216
-- Data for Name: chats; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 4815 (class 0 OID 16512)
-- Dependencies: 220
-- Data for Name: chats_info; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 4814 (class 0 OID 16497)
-- Dependencies: 219
-- Data for Name: file_names; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 4816 (class 0 OID 16522)
-- Dependencies: 221
-- Data for Name: files; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 4812 (class 0 OID 16454)
-- Dependencies: 217
-- Data for Name: useraccdata; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 4810 (class 0 OID 16429)
-- Dependencies: 215
-- Data for Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.users (nickname, password, id) VALUES ('test', '12345', 1);


--
-- TOC entry 4823 (class 0 OID 0)
-- Dependencies: 218
-- Name: users_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.users_id_seq', 2, false);


--
-- TOC entry 4656 (class 2606 OID 16438)
-- Name: users users_nickname_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_nickname_key UNIQUE (nickname);


--
-- TOC entry 4658 (class 2606 OID 16469)
-- Name: users users_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (id);


--
-- TOC entry 4664 (class 2606 OID 16517)
-- Name: chats_info chats_info_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.chats_info
    ADD CONSTRAINT chats_info_user_id_fkey FOREIGN KEY (user_id) REFERENCES public.users(id);


--
-- TOC entry 4662 (class 2606 OID 16502)
-- Name: file_names file_names_user_1_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.file_names
    ADD CONSTRAINT file_names_user_1_id_fkey FOREIGN KEY (user_1_id) REFERENCES public.users(id);


--
-- TOC entry 4663 (class 2606 OID 16507)
-- Name: file_names file_names_user_2_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.file_names
    ADD CONSTRAINT file_names_user_2_id_fkey FOREIGN KEY (user_2_id) REFERENCES public.users(id);


--
-- TOC entry 4665 (class 2606 OID 16527)
-- Name: files files_user_1_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.files
    ADD CONSTRAINT files_user_1_id_fkey FOREIGN KEY (user_1_id) REFERENCES public.users(id);


--
-- TOC entry 4666 (class 2606 OID 16532)
-- Name: files files_user_2_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.files
    ADD CONSTRAINT files_user_2_id_fkey FOREIGN KEY (user_2_id) REFERENCES public.users(id);


--
-- TOC entry 4659 (class 2606 OID 16477)
-- Name: chats fk_user1_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.chats
    ADD CONSTRAINT fk_user1_id FOREIGN KEY (user_1_id) REFERENCES public.users(id);


--
-- TOC entry 4660 (class 2606 OID 16482)
-- Name: chats fk_user2_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.chats
    ADD CONSTRAINT fk_user2_id FOREIGN KEY (user_2_id) REFERENCES public.users(id);


--
-- TOC entry 4661 (class 2606 OID 16492)
-- Name: useraccdata fk_userid_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.useraccdata
    ADD CONSTRAINT fk_userid_id FOREIGN KEY (user_id) REFERENCES public.users(id) ON DELETE CASCADE;


-- Completed on 2024-03-15 00:44:12

--
-- PostgreSQL database dump complete
--

